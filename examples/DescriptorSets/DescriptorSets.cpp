#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "ExampleBase.h"

#include "Window.hpp"
#include "VkInitializers.hpp"

using namespace example;

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
	void preparePipelines();
	void buildCommandBuffers();

	struct Cube {
		struct Matrices {
			glm::mat4 projection;
			glm::mat4 view;
			glm::mat4 model;
		} matrices;
		vkw::DescriptorSet descriptorSet;
		Texture2D texture;
		vkw::Buffer uniformBuffer;
		glm::vec3 rotation;
	};
	std::array<Cube, 2> cubes;

	Mesh cubeMesh;

	vkw::Memory uniformBufferMemory;
	vkw::DescriptorSetLayout descriptorSetLayout;
	vkw::DescriptorPool descriptorPool;
	vkw::PipelineLayout pipelineLayout;
	vkw::GraphicsPipeline pipeline;
};


VkwExample::VkwExample(Window & window) : ExampleBase(window) {
	auto func = [](const vkw::PhysicalDevice& phys, const vkw::Surface& surf) {
		return phys.features.samplerAnisotropy;
	};

	InitInfo initInfo = InitInfo();
	initInfo.deviceSuitableFkt.push_back(func);
	initInfo.deviceFeatures.samplerAnisotropy = VK_TRUE;
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
	preparePipelines();
	buildCommandBuffers();
}

void VkwExample::loadAssets()
{
	Mesh::LoadInfo meshInfo(modelPath() + "cube.dae", { {VERTEX_COMPONENT_POSITION, VERTEX_COMPONENT_NORMAL, VERTEX_COMPONENT_UV, VERTEX_COMPONENT_COLOR} }, &cubeMesh);
	meshloader.loadFromFile({ meshInfo });

	Texture2D::CreateInfo textureInfo1(texturePath() + "crate01_color_height_rgba.ktx", VK_FORMAT_R8G8B8A8_UNORM, &cubes[0].texture);
	Texture2D::CreateInfo textureInfo2(texturePath() + "crate02_color_height_rgba.ktx", VK_FORMAT_R8G8B8A8_UNORM, &cubes[1].texture);
	textureLoader.loadFromFile({ textureInfo1, textureInfo2 });
}

void VkwExample::prepareUniformBuffers()
{
	uniformBufferMemory.setFlags(VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

	for (auto & x : cubes) {
		x.uniformBuffer.createBuffer(VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, sizeof(Cube::Matrices));
		uniformBufferMemory.setMemoryTypeBitsBuffer(x.uniformBuffer);
	}

	uniformBufferMemory.allocateMemory();

	for (auto & x : cubes) uniformBufferMemory.bindBufferToMemory(x.uniformBuffer);
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
	descriptorPoolCreateInfo.uniformBufferCount = 1 + static_cast<uint32_t>(cubes.size());  // Uniform buffers : 1 for scene and 1 per object (scene and local matrices)
	descriptorPoolCreateInfo.combinedImageSamplerCount = static_cast<uint32_t>(cubes.size()); 	// Combined image samples : 1 per mesh texture
	descriptorPoolCreateInfo.maxSets = static_cast<uint32_t>(cubes.size());
	
	descriptorPool.createDescriptorPool(descriptorPoolCreateInfo);


	for (auto & cube : cubes) {
		cube.descriptorSet.allocateDescriptorSet(descriptorPool, descriptorSetLayout);

		VkDescriptorBufferInfo bufferInfo = cube.uniformBuffer.bufferInfo();
		vkw::DescriptorSet::WriteInfo bufferWriteInfo = {};
		bufferWriteInfo.dstBinding = 0;
		bufferWriteInfo.descriptorCount = 1;
		bufferWriteInfo.pBufferInfo = &bufferInfo;

		VkDescriptorImageInfo imageInfo = cube.texture.descriptorInfo();
		vkw::DescriptorSet::WriteInfo imageWriteInfo = {};
		imageWriteInfo.dstBinding = 1;
		imageWriteInfo.descriptorCount = 1;
		imageWriteInfo.pImageInfo = &imageInfo;

		cube.descriptorSet.update({ bufferWriteInfo, imageWriteInfo }, {});
	}
}

void VkwExample::preparePipelines()
{
	pipelineLayout.createPipelineLayout({ descriptorSetLayout }, {});

	const std::vector<VkDynamicState> dynamicStateEnables = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };

	VkPipelineInputAssemblyStateCreateInfo inputAssemblyState = initializers::pipelineInputAssemblyStateCreateInfo(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, VK_FALSE);
	VkPipelineRasterizationStateCreateInfo rasterizationState = initializers::pipelineRasterizationStateCreateInfo(VK_POLYGON_MODE_FILL, VK_CULL_MODE_BACK_BIT, VK_FRONT_FACE_CLOCKWISE);
	VkPipelineColorBlendAttachmentState blendAttachmentState = vkw::init::pipelineColorBlendAttachmentState(0xf, VK_FALSE);
	VkPipelineColorBlendStateCreateInfo colorBlendState = initializers::pipelineColorBlendStateCreateInfo(1, &blendAttachmentState);
	VkPipelineDepthStencilStateCreateInfo depthStencilState = initializers::pipelineDepthStencilStateCreateInfo(VK_TRUE, VK_TRUE, VK_COMPARE_OP_LESS_OR_EQUAL);
	VkPipelineViewportStateCreateInfo viewportState = initializers::pipelineViewportStateCreateInfo(1, 1);
	VkPipelineMultisampleStateCreateInfo multisampleState = initializers::pipelineMultisampleStateCreateInfo(VK_SAMPLE_COUNT_1_BIT);
	VkPipelineDynamicStateCreateInfo dynamicState = initializers::pipelineDynamicStateCreateInfo(dynamicStateEnables);

	const std::vector<VkVertexInputBindingDescription> vertexBindings = { cubeMesh.vertexBinding(0) };
	const std::vector<VkVertexInputAttributeDescription> vertexInputAttributes = cubeMesh.vertexAttributes(0);

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
	
	vkw::ShaderModule vertexShader(shaderPath() + "cube.vert.spv", VK_SHADER_STAGE_VERTEX_BIT);
	vkw::ShaderModule fragShader(shaderPath() + "cube.frag.spv", VK_SHADER_STAGE_FRAGMENT_BIT);
	pipelineCreateInfo.shaderStages = { vertexShader.shaderStageInfo(), fragShader.shaderStageInfo() };

	pipeline.createPipeline(pipelineCreateInfo);
}

void VkwExample::buildCommandBuffers()
{
	drawCommandBuffers.resize(swapChain.imageCount);
	vkw::CommandBuffer::allocateCommandBuffers(drawCommandBuffers, graphicsCommandPool);

	for (uint32_t i = 0; i < drawCommandBuffers.size(); i++) {
		VkClearValue clearValues[2];
		clearValues[0] = { 0.0f, 0.0f, 0.0f, 1.0f };
		clearValues[1].depthStencil = { 1.0f , 0 };

		VkRenderPassBeginInfo beginnInfo = vkw::init::renderPassBeginInfo();
		beginnInfo.renderPass = renderPass;
		beginnInfo.renderArea = { { 0,0 }, swapChain.extent };
		beginnInfo.framebuffer = renderFrameBuffers[i];
		beginnInfo.clearValueCount = 2;
		beginnInfo.pClearValues = clearValues;

		drawCommandBuffers[i].beginCommandBuffer(VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT);

			vkCmdBeginRenderPass(drawCommandBuffers[i], &beginnInfo, VK_SUBPASS_CONTENTS_INLINE);

			VkDeviceSize offsets[] = { 0 };
			VkBuffer buffers[] = { cubeMesh.vertexBuffer };
			vkCmdBindVertexBuffers(drawCommandBuffers[i], 0, 1, buffers, offsets);
			vkCmdBindIndexBuffer(drawCommandBuffers[i], cubeMesh.indexBuffer, 0, VK_INDEX_TYPE_UINT32);

			VkViewport viewport = vkw::init::viewport(swapChain.extent);
			vkCmdSetViewport(drawCommandBuffers[i], 0, 1, &viewport);

			VkRect2D scissor = { { 0,0 }, swapChain.extent };
			vkCmdSetScissor(drawCommandBuffers[i], 0, 1, &scissor);

			vkCmdBindPipeline(drawCommandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);

			for (auto cube : cubes) {
				vkCmdBindDescriptorSets(drawCommandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, cube.descriptorSet.get(), 0, nullptr);
				vkCmdDrawIndexed(drawCommandBuffers[i], cubeMesh.indexCount, 1, 0, 0, 0);
			}

			vkCmdEndRenderPass(drawCommandBuffers[i]);

		drawCommandBuffers[i].endCommandBuffer();
	}
}


void VkwExample::nextFrame() {
	static auto startTime = std::chrono::high_resolution_clock::now();

	auto currentTime = std::chrono::high_resolution_clock::now();
	float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

	cubes[0].matrices.model = glm::translate(glm::mat4(1.0f), glm::vec3(-2.0f, 0.0f, 0.0f));
	cubes[1].matrices.model = glm::translate(glm::mat4(1.0f), glm::vec3(1.5f, 0.5f, 0.0f));

	cubes[0].matrices.model = glm::rotate(cubes[0].matrices.model, glm::radians(90.f * time),glm::vec3(0.0f, 1.0f, 0.0f));
	cubes[1].matrices.model = glm::rotate(cubes[1].matrices.model, glm::radians(70.f * time),glm::vec3(1.0f, 0.0f, 0.0f));

	for (auto & cube : cubes) {
		cube.matrices.view = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, -5));
		cube.matrices.projection = glm::perspective(glm::radians(60.0f), swapChain.extent.width / (float)swapChain.extent.height, 0.1f, 10.0f);
		
		cube.uniformBuffer.write(&cube.matrices, sizeof(cube.matrices));
	}

	renderFrame();
	
}


int main() {
	GlfwWindow window(1400, 800);

	auto example = VkwExample(window);

	while (!glfwWindowShouldClose(window)) {
		example.nextFrame();
		glfwPollEvents();
	}
}

