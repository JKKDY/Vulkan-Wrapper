#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <random>
#define _USE_MATH_DEFINES
#include <math.h>

#include "ExampleBase.h"

#include "Window.hpp"
#include "VkInitializers.hpp"

#define OBJECT_INSTANCES 125

using namespace vkex;

class VkwExample : public ExampleBase {
public:
	VkwExample(Window & window);
	~VkwExample();
	void nextFrame() override;
private:
	void setup() override;
	void generateCube();
	void prepareUniformBuffers();
	void setupDescriptors();
	void preparePipelines();
	void buildCommandBuffers();


	struct Vertex {
		float pos[3];
		float color[3];
	};

	glm::vec3 rotations[OBJECT_INSTANCES];
	glm::vec3 rotationSpeeds[OBJECT_INSTANCES];
	size_t dynamicAlignment;

	glm::mat4 * modelMatricies;

	vkw::Memory cubeMemory;
	vkw::Buffer vertexBuffer;
	vkw::Buffer indexBuffer;
	uint32_t indexCount;

	vkw::Memory uniformBuffersMemory;
	vkw::Buffer viewUniformBuffer;
	vkw::Buffer dynamicUniformBuffer;

	vkw::GraphicsPipeline pipeline;
	vkw::DescriptorSetLayout descriptorSetLayout;
	vkw::DescriptorPool descriptorPool;
	vkw::DescriptorSet descriptorSet;
	vkw::PipelineLayout pipelineLayout;
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
	camera.perspective = glm::perspective(glm::radians(60.0f), (float)swapChain.extent.width / (float)swapChain.extent.height, 0.1f, 256.0f);
	camera.view = glm::lookAt(glm::vec3(0.0f, 0.0f, -30.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	generateCube();
	prepareUniformBuffers();
	setupDescriptors();
	preparePipelines();
	buildCommandBuffers();
}


void VkwExample::generateCube()
{
	std::vector<Vertex> vertices = {
			{ { -1.0f, -1.0f,  1.0f },{ 1.0f, 0.0f, 0.0f } },
			{ {  1.0f, -1.0f,  1.0f },{ 0.0f, 1.0f, 0.0f } },
			{ {  1.0f,  1.0f,  1.0f },{ 0.0f, 0.0f, 1.0f } },
			{ { -1.0f,  1.0f,  1.0f },{ 0.0f, 0.0f, 0.0f } },
			{ { -1.0f, -1.0f, -1.0f },{ 1.0f, 0.0f, 0.0f } },
			{ {  1.0f, -1.0f, -1.0f },{ 0.0f, 1.0f, 0.0f } },
			{ {  1.0f,  1.0f, -1.0f },{ 0.0f, 0.0f, 1.0f } },
			{ { -1.0f,  1.0f, -1.0f },{ 0.0f, 0.0f, 0.0f } },
	};

	std::vector<uint32_t> indices = {
		0,1,2, 2,3,0, 1,5,6, 6,2,1, 7,6,5, 5,4,7, 4,0,3, 3,7,4, 4,5,1, 1,0,4, 3,2,6, 6,7,3,
	};
	indexCount = static_cast<uint32_t>(indices.size());

	vkw::Memory stagingMemory(VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
	vkw::Buffer vertexStagingBuffer(VK_BUFFER_USAGE_TRANSFER_SRC_BIT, vertices.size() * sizeof(Vertex));
	vkw::Buffer	indexStagingBuffer(VK_BUFFER_USAGE_TRANSFER_SRC_BIT, indices.size() * sizeof(uint32_t));
	stagingMemory.allocateMemory({ vertexStagingBuffer, indexStagingBuffer });

	vertexBuffer.createBuffer(VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, vertices.size() * sizeof(Vertex));
	indexBuffer.createBuffer(VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, indices.size() * sizeof(uint32_t));
	cubeMemory.allocateMemory(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, { vertexBuffer, indexBuffer });

	vertexStagingBuffer.write(vertices.data(), vertices.size() * sizeof(Vertex));
	indexStagingBuffer.write(indices.data(), indices.size() * sizeof(uint32_t));

	vertexBuffer.copyFromBuffer(vertexStagingBuffer);
	indexBuffer.copyFromBuffer(indexStagingBuffer);
}

void VkwExample::prepareUniformBuffers() {
	dynamicAlignment = vkw::tools::allignementOf(sizeof(glm::mat4), physicalDevice.properties.limits.minUniformBufferOffsetAlignment);
	size_t bufferSize = dynamicAlignment * OBJECT_INSTANCES;
	modelMatricies = reinterpret_cast<glm::mat4*>(vkw::tools::alignedAlloc(bufferSize, dynamicAlignment));

	viewUniformBuffer.createBuffer(VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, sizeof(glm::mat4) );
	dynamicUniformBuffer.createBuffer(VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, bufferSize);
	uniformBuffersMemory.allocateMemory(VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, { viewUniformBuffer, dynamicUniformBuffer });

	glm::mat4 viewProj = camera.perspective * camera.view;
	viewUniformBuffer.write(&viewProj, sizeof(glm::mat4));

	std::default_random_engine rndEngine((unsigned)time(nullptr));
	std::normal_distribution<float> rndDist(-1.0f, 1.0f);
	for (uint32_t i = 0; i < OBJECT_INSTANCES; i++) {
		rotations[i] = glm::vec3(rndDist(rndEngine), rndDist(rndEngine), rndDist(rndEngine)) * 2.0f * (float)M_PI;
		rotationSpeeds[i] = glm::vec3(rndDist(rndEngine), rndDist(rndEngine), rndDist(rndEngine));
	}
}

void VkwExample::setupDescriptors() {
	std::vector<VkDescriptorSetLayoutBinding> setLayoutBindings =
	{
		initializers::descriptorSetLayoutBinding(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT, 0),
		initializers::descriptorSetLayoutBinding(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, VK_SHADER_STAGE_VERTEX_BIT, 1),
		initializers::descriptorSetLayoutBinding(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT, 2)///---------
	};
	descriptorSetLayout.createDescriptorSetLayout(setLayoutBindings);

	vkw::DescriptorPool::CreateInfo2 poolCreateInfo = {};
	poolCreateInfo.uniformBufferDynamicCount = 1;
	poolCreateInfo.uniformBufferCount = 1;
	poolCreateInfo.combinedImageSamplerCount = 1;///----------
	poolCreateInfo.maxSets = 2;
	descriptorPool.createDescriptorPool(poolCreateInfo);

	descriptorSet.allocateDescriptorSet(descriptorPool, descriptorSetLayout);

	VkDescriptorBufferInfo bufferInfoView = viewUniformBuffer.bufferInfo();
	vkw::DescriptorSet::WriteInfo viewWriteInfo = {};
	viewWriteInfo.dstBinding = 0;
	viewWriteInfo.descriptorCount = 1;
	viewWriteInfo.pBufferInfo = &bufferInfoView;

	VkDescriptorBufferInfo bufferInfoDynamic = dynamicUniformBuffer.bufferInfo();
	vkw::DescriptorSet::WriteInfo dynamicWriteInfo = {};
	dynamicWriteInfo.dstBinding = 1;
	dynamicWriteInfo.descriptorCount = 1;
	dynamicWriteInfo.pBufferInfo = &bufferInfoDynamic;

	descriptorSet.update({ dynamicWriteInfo, viewWriteInfo }, {});
}

void VkwExample::preparePipelines() {
	pipelineLayout.createPipelineLayout({ descriptorSetLayout }, {});

	const std::vector<VkDynamicState> dynamicStateEnables = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };

	VkPipelineInputAssemblyStateCreateInfo inputAssemblyState = initializers::pipelineInputAssemblyStateCreateInfo(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, VK_FALSE);
	VkPipelineRasterizationStateCreateInfo rasterizationState = initializers::pipelineRasterizationStateCreateInfo(VK_POLYGON_MODE_FILL, VK_CULL_MODE_NONE, VK_FRONT_FACE_COUNTER_CLOCKWISE);
	VkPipelineColorBlendAttachmentState blendAttachmentState = vkw::init::pipelineColorBlendAttachmentState(0xf, VK_FALSE);
	VkPipelineColorBlendStateCreateInfo colorBlendState = initializers::pipelineColorBlendStateCreateInfo(1, &blendAttachmentState);
	VkPipelineDepthStencilStateCreateInfo depthStencilState = initializers::pipelineDepthStencilStateCreateInfo(VK_TRUE, VK_TRUE, VK_COMPARE_OP_LESS_OR_EQUAL);
	VkPipelineViewportStateCreateInfo viewportState = initializers::pipelineViewportStateCreateInfo(1, 1);
	VkPipelineMultisampleStateCreateInfo multisampleState = initializers::pipelineMultisampleStateCreateInfo(VK_SAMPLE_COUNT_1_BIT);
	VkPipelineDynamicStateCreateInfo dynamicState = initializers::pipelineDynamicStateCreateInfo(dynamicStateEnables);

	const std::vector<VkVertexInputBindingDescription> vertexBindings = {
			initializers::vertexInputBindingDescription(0, sizeof(Vertex), VK_VERTEX_INPUT_RATE_VERTEX),
	};

	const std::vector<VkVertexInputAttributeDescription> vertexInputAttributes = {
		initializers::vertexInputAttributeDescription(0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, pos)),
		initializers::vertexInputAttributeDescription(0, 1, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, color))
	};

	VkPipelineVertexInputStateCreateInfo vertexInputState = initializers::pipelineVertexInputStateCreateInfo(vertexBindings, vertexInputAttributes);

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

				for (uint32_t j = 0; j < OBJECT_INSTANCES; j++)
				{
					uint32_t dynamicOffset = j * static_cast<uint32_t>(dynamicAlignment);
					vkCmdBindDescriptorSets(drawCommandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, descriptorSet.get(), 1, &dynamicOffset);

					vkCmdDrawIndexed(drawCommandBuffers[i], indexCount, 1, 0, 0, 0);
				}

			vkCmdEndRenderPass(drawCommandBuffers[i]);

		drawCommandBuffers[i].endCommandBuffer();
	}
}

void VkwExample::nextFrame() {
	static auto startTime = std::chrono::high_resolution_clock::now();

	auto currentTime = std::chrono::high_resolution_clock::now();
	float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

	// Dynamic ubo with per-object model matrices indexed by offsets in the command buffer
	uint32_t dim = static_cast<uint32_t>(pow(OBJECT_INSTANCES, (1.0f / 3.0f)));
	glm::vec3 offset(5.0f);

	for (uint32_t x = 0; x < dim; x++)
	{
		for (uint32_t y = 0; y < dim; y++)
		{
			for (uint32_t z = 0; z < dim; z++)
			{
				uint32_t index = x * dim * dim + y * dim + z;

				// Aligned offset
				glm::mat4* modelMat = (glm::mat4*)(((uint64_t)modelMatricies + (index * dynamicAlignment)));

				// Update rotations
				rotations[index] = time * rotationSpeeds[index];

				// Update matrices
				glm::vec3 pos = glm::vec3(-((dim * offset.x) / 2.0f) + offset.x / 2.0f + x * offset.x, -((dim * offset.y) / 2.0f) + offset.y / 2.0f + y * offset.y, -((dim * offset.z) / 2.0f) + offset.z / 2.0f + z * offset.z);
				*modelMat = glm::translate(glm::mat4(1.0f), pos);
				*modelMat = glm::rotate(*modelMat, rotations[index].x, glm::vec3(1.0f, 1.0f, 0.0f));
				*modelMat = glm::rotate(*modelMat, rotations[index].y, glm::vec3(0.0f, 1.0f, 0.0f));
				*modelMat = glm::rotate(*modelMat, rotations[index].z, glm::vec3(0.0f, 0.0f, 1.0f));
			}
		}
	}

	dynamicUniformBuffer.write(modelMatricies, dynamicUniformBuffer.size);
	//// Flush to make changes visible to the host 
	//VkMappedMemoryRange memoryRange = vks::initializers::mappedMemoryRange();
	//memoryRange.memory = uniformBuffers.dynamic.memory;
	//memoryRange.size = uniformBuffers.dynamic.size;
	//vkFlushMappedMemoryRanges(device, 1, &memoryRange);

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

