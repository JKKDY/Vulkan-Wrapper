#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "ExampleBase.h"

#include "Window.hpp"
#include "VkInitializers.hpp"

using namespace vkex;

class VkwExample : public ExampleBase {
public:
	VkwExample(Window & window);
	~VkwExample();
	void nextFrame() override;
private:
	void setup() override;
	void loadAssets();
	void prepareUniformBuffers();
	void setupDescriptors();
	void createPipelines();
	void buildCommandBuffers();

	struct UBO {
		glm::mat4 proj;
		glm::mat4 modelView;	
		glm::mat4 model;
	}ubo;

	struct Vertex {
		glm::vec3 pos;
		glm::vec2 texCoord;
	};

	vkw::Memory modelMemory;
	vkw::Buffer indexBuffer;
	vkw::Buffer vertexBuffer;
	uint32_t indexCount;
	
	vkw::Memory uniformBufferMemory;
	vkw::Buffer uniformBuffer;

	vkw::DescriptorPool descriptorPool;
	vkw::DescriptorSetLayout descriptorSetLayout;
	vkw::DescriptorSet descriptorSet;

	Texture2D texture;

	vkw::PipelineLayout pipelineLayout;
	vkw::GraphicsPipeline pipeline;
};

VkwExample::VkwExample(Window & window) : ExampleBase(window) {
	auto func = [](const vkw::PhysicalDevice& phys, const vkw::Surface& surf) {
		return true;
	};

	InitInfo initInfo = InitInfo();
	initInfo.deviceSuitableFkt.push_back(func);
	initVulkan(initInfo);
	setup();
}

VkwExample::~VkwExample()
{
	vkDeviceWaitIdle(device);
	for (auto & x : drawCommandBuffers) x.destroyObject();
}

void VkwExample::setup() {
	loadAssets();
	prepareUniformBuffers();
	setupDescriptors();
	createPipelines();
	buildCommandBuffers();
}

void VkwExample::loadAssets()
{
	Texture2D::CreateInfo textureInfo(texturePath() + "chalet.jpg", VK_FORMAT_R8G8B8A8_UNORM, &texture);
	textureLoader.loadFromFile({ textureInfo });

	std::vector<Vertex> vertices;
	std::vector<uint32_t> indices;

	Assimp::Importer importer;
	const aiScene * pScene = importer.ReadFile(modelPath() + "samplescene.dae", aiProcess_FlipWindingOrder | aiProcess_Triangulate | aiProcess_PreTransformVertices | aiProcess_CalcTangentSpace | aiProcess_GenSmoothNormals | aiProcess_FlipUVs);

	const aiVector3D Zero3D(0.0f, 0.0f, 0.0f);
	for (unsigned int i = 0; i < pScene->mNumMeshes; i++)
	{
		const aiMesh* paiMesh = pScene->mMeshes[i];

		aiColor3D pColor(0.f, 0.f, 0.f);
		pScene->mMaterials[paiMesh->mMaterialIndex]->Get(AI_MATKEY_COLOR_DIFFUSE, pColor);

		// load verticies
		for (unsigned int j = 0; j < paiMesh->mNumVertices; j++)
		{
			const aiVector3D* pPos = &(paiMesh->mVertices[j]);
			const aiVector3D* pNormal = &(paiMesh->mNormals[j]);
			const aiVector3D* pTexCoord = (paiMesh->HasTextureCoords(0)) ? &(paiMesh->mTextureCoords[0][j]) : &Zero3D;
			const aiVector3D* pTangent = (paiMesh->HasTangentsAndBitangents()) ? &(paiMesh->mTangents[j]) : &Zero3D;
			const aiVector3D* pBiTangent = (paiMesh->HasTangentsAndBitangents()) ? &(paiMesh->mBitangents[j]) : &Zero3D;

			Vertex vertex = {};
			vertex.pos = { pPos->x, -pPos->y, pPos->z };
			vertex.texCoord = { pTexCoord->x, pTexCoord->y };

			vertices.push_back(vertex);
		}

		// load indicies
		uint32_t indexBase = static_cast<uint32_t>(indices.size());
		for (unsigned int j = 0; j < paiMesh->mNumFaces; j++)
		{
			const aiFace& Face = paiMesh->mFaces[j];
			if (Face.mNumIndices != 3)
				continue;
			indices.push_back(indexBase + Face.mIndices[0]);
			indices.push_back(indexBase + Face.mIndices[1]);
			indices.push_back(indexBase + Face.mIndices[2]);
		}
	}

	indexCount = static_cast<uint32_t>(indices.size());

	vkw::Memory stagingMemory(VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
	vkw::Buffer vertexStagingBuffer(VK_BUFFER_USAGE_TRANSFER_SRC_BIT, vertices.size() * sizeof(Vertex));
	vkw::Buffer indexStagingBuffer(VK_BUFFER_USAGE_TRANSFER_SRC_BIT, indices.size() * sizeof(uint32_t));
	stagingMemory.allocateMemory({ vertexStagingBuffer, indexStagingBuffer });

	vertexStagingBuffer.write(vertices.data(), vertices.size() * sizeof(Vertex));
	indexStagingBuffer.write(indices.data(), indices.size() * sizeof(uint32_t));

	vertexBuffer.createBuffer(VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, vertices.size() * sizeof(Vertex));
	indexBuffer.createBuffer(VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, indices.size() * sizeof(uint32_t));
	modelMemory.allocateMemory({ vertexBuffer, indexBuffer });

	vertexBuffer.copyFromBuffer(vertexStagingBuffer);
	indexBuffer.copyFromBuffer(indexStagingBuffer);
}

void VkwExample::prepareUniformBuffers()
{
	uniformBuffer.createBuffer(VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, sizeof(UBO));

	uniformBufferMemory.allocateMemory(VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, { uniformBuffer }, {});
}

void VkwExample::setupDescriptors()
{
	VkDescriptorSetLayoutBinding uniformLayoutBinding = {};
	uniformLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	uniformLayoutBinding.binding = 0;
	uniformLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
	uniformLayoutBinding.descriptorCount = 1;

	VkDescriptorSetLayoutBinding samplerLayoutBinding = {};
	samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	samplerLayoutBinding.binding = 1;
	samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
	samplerLayoutBinding.descriptorCount = 1;

	descriptorSetLayout.createDescriptorSetLayout({ uniformLayoutBinding ,samplerLayoutBinding });


	vkw::DescriptorPool::CreateInfo2 descriptorPoolCreateInfo = {};
	descriptorPoolCreateInfo.uniformBufferCount = 1;
	descriptorPoolCreateInfo.combinedImageSamplerCount = 1;
	descriptorPoolCreateInfo.maxSets = 1;
	descriptorPool.createDescriptorPool(descriptorPoolCreateInfo);


	descriptorSet.allocateDescriptorSet(descriptorPool, descriptorSetLayout);

	VkDescriptorBufferInfo bufferInfo = uniformBuffer.bufferInfo();
	vkw::DescriptorSet::WriteInfo bufferWriteInfo = {};
	bufferWriteInfo.dstBinding = 0;
	bufferWriteInfo.descriptorCount = 1;
	bufferWriteInfo.pBufferInfo = &bufferInfo;

	VkDescriptorImageInfo imageInfo = texture.descriptorInfo();
	vkw::DescriptorSet::WriteInfo imageWriteInfo = {};
	imageWriteInfo.dstBinding = 1;
	imageWriteInfo.descriptorCount = 1;
	imageWriteInfo.pImageInfo = &imageInfo;

	descriptorSet.update({ bufferWriteInfo, imageWriteInfo }, {});
}

void VkwExample::createPipelines()
{
	pipelineLayout.createPipelineLayout({ descriptorSetLayout }, {});

	const std::vector<VkDynamicState> dynamicStateEnables = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };

	VkPipelineInputAssemblyStateCreateInfo inputAssemblyState = initializers::pipelineInputAssemblyStateCreateInfo(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, VK_FALSE);
	VkPipelineRasterizationStateCreateInfo rasterizationState = initializers::pipelineRasterizationStateCreateInfo(VK_POLYGON_MODE_FILL, VK_CULL_MODE_BACK_BIT, VK_FRONT_FACE_COUNTER_CLOCKWISE);
	VkPipelineColorBlendAttachmentState blendAttachmentState = vkw::init::pipelineColorBlendAttachmentState(0xf, VK_FALSE);
	VkPipelineViewportStateCreateInfo viewportState = initializers::pipelineViewportStateCreateInfo(1, 1);
	VkPipelineColorBlendStateCreateInfo colorBlendState = initializers::pipelineColorBlendStateCreateInfo(1, &blendAttachmentState);
	VkPipelineDepthStencilStateCreateInfo depthStencilState = initializers::pipelineDepthStencilStateCreateInfo(VK_TRUE, VK_TRUE, VK_COMPARE_OP_LESS_OR_EQUAL);
	VkPipelineMultisampleStateCreateInfo multisampleState = initializers::pipelineMultisampleStateCreateInfo(VK_SAMPLE_COUNT_1_BIT);
	VkPipelineDynamicStateCreateInfo dynamicState = initializers::pipelineDynamicStateCreateInfo(dynamicStateEnables);

	std::vector<VkVertexInputBindingDescription> vertexBindings(1);
	vertexBindings[0].binding = 0;
	vertexBindings[0].stride = sizeof(Vertex);
	vertexBindings[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

	std::vector<VkVertexInputAttributeDescription> vertexInputAttributes(2);
	vertexInputAttributes[0].binding = 0;
	vertexInputAttributes[0].location = 0;
	vertexInputAttributes[0].format = VK_FORMAT_R32G32B32_SFLOAT;
	vertexInputAttributes[0].offset = offsetof(Vertex, pos);

	vertexInputAttributes[1].binding = 0;
	vertexInputAttributes[1].location = 1;
	vertexInputAttributes[1].format = VK_FORMAT_R32G32_SFLOAT;
	vertexInputAttributes[1].offset = offsetof(Vertex, texCoord);

	VkPipelineVertexInputStateCreateInfo vertexInputState = vkw::init::pipelineVertexInputStateCreateInfo();
	vertexInputState.vertexBindingDescriptionCount = static_cast<uint32_t>(vertexBindings.size());
	vertexInputState.pVertexBindingDescriptions = vertexBindings.data();
	vertexInputState.vertexAttributeDescriptionCount = static_cast<uint32_t>(vertexInputAttributes.size());
	vertexInputState.pVertexAttributeDescriptions = vertexInputAttributes.data();

	vkw::GraphicsPipeline::CreateInfo pipelineCreateInfo = {};
	pipelineCreateInfo.inputAssemblyState = &inputAssemblyState;
	pipelineCreateInfo.rasterizationState = &rasterizationState;
	pipelineCreateInfo.colorBlendState = &colorBlendState;
	pipelineCreateInfo.depthStencilState = &depthStencilState;
	pipelineCreateInfo.viewportState = &viewportState;
	pipelineCreateInfo.multisampleState = &multisampleState;
	pipelineCreateInfo.dynamicState = &dynamicState;
	pipelineCreateInfo.vertexInputState = &vertexInputState;
	pipelineCreateInfo.renderPass = renderPass;
	pipelineCreateInfo.subPass = 0;
	pipelineCreateInfo.layout = pipelineLayout;
	pipelineCreateInfo.cache = pipelineCache;

	vkw::ShaderModule vertexShader(shaderPath() + "base.vert.spv", VK_SHADER_STAGE_VERTEX_BIT);
	vkw::ShaderModule fragShader(shaderPath() + "base.frag.spv", VK_SHADER_STAGE_FRAGMENT_BIT);
	pipelineCreateInfo.shaderStages = { vertexShader.shaderStageInfo(), fragShader.shaderStageInfo() };

	pipeline.createPipeline(pipelineCreateInfo);
}

void VkwExample::buildCommandBuffers() {
	drawCommandBuffers.resize(swapChain.imageCount);
	vkw::CommandBuffer::allocateCommandBuffers(drawCommandBuffers, graphicsCommandPool);

	VkClearValue clearValues[2];
	clearValues[0] = { 0.0f, 0.0f, 0.0f, 1.0f };
	clearValues[1].depthStencil = { 1.0f , 0 };

	VkRenderPassBeginInfo beginnInfo = vkw::init::renderPassBeginInfo();
	beginnInfo.renderPass = renderPass;
	beginnInfo.renderArea = vkw::init::rect2D(swapChain.extent);
	beginnInfo.clearValueCount = 2;
	beginnInfo.pClearValues = clearValues;

	for (uint32_t i = 0; i < drawCommandBuffers.size(); i++) {
		beginnInfo.framebuffer = renderFrameBuffers[i];

		drawCommandBuffers[i].beginCommandBuffer(VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT);

		vkCmdBeginRenderPass(drawCommandBuffers[i], &beginnInfo, VK_SUBPASS_CONTENTS_INLINE);

		VkViewport viewport = vkw::init::viewport(swapChain.extent);
		vkCmdSetViewport(drawCommandBuffers[i], 0, 1, &viewport);

		VkRect2D scissor = { { 0,0 }, swapChain.extent };
		vkCmdSetScissor(drawCommandBuffers[i], 0, 1, &scissor);

		VkDeviceSize offsets[] = { 0 };
		VkBuffer buffers[] = { vertexBuffer };
		vkCmdBindVertexBuffers(drawCommandBuffers[i], 0, 1, buffers, offsets);
		vkCmdBindIndexBuffer(drawCommandBuffers[i], indexBuffer, 0, VK_INDEX_TYPE_UINT32);

		vkCmdBindPipeline(drawCommandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
		vkCmdBindDescriptorSets(drawCommandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, descriptorSet.get() , 0, nullptr);
		vkCmdDrawIndexed(drawCommandBuffers[i], indexCount, 1, 0, 0, 0);

		vkCmdEndRenderPass(drawCommandBuffers[i]);

		drawCommandBuffers[i].endCommandBuffer();
	}
}


void VkwExample::nextFrame() {
	static auto startTime = std::chrono::high_resolution_clock::now();

	auto currentTime = std::chrono::high_resolution_clock::now();
	float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

	UBO ubo = {};
	ubo.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(60.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	ubo.modelView = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	ubo.proj = glm::perspective(glm::radians(45.0f), swapChain.extent.width / (float)swapChain.extent.height, 0.1f, 10.0f);
	ubo.proj[1][1] *= -1;

	uniformBuffer.write(&ubo, sizeof(UBO));

	renderFrame();
}


int main() {
	GlfwWindow window(1000, 800);

	auto example = VkwExample(window);

	while (!glfwWindowShouldClose(window)) {
		example.nextFrame();
		glfwPollEvents();
	}
}

