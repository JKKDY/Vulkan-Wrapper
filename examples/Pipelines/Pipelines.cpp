#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "ExampleBase.h"

#include "Window.hpp"
#include "VkInitializers.hpp"


using namespace vkex;

class PipelineExample : public ExampleBase {
public:
	PipelineExample(Window & window);
	~PipelineExample();
	void nextFrame() override;
private:
	void setup() override;
	void loadModels();
	void createUBO();
	void createDescriptorSets();
	void createPipelines();
	void setupRenderCommanBuffers();

	struct UBO {
		glm::mat4 model;
		glm::mat4 view;
		glm::mat4 proj;
		glm::vec3 light;
	}ubo;

	struct PipelineWithShaders {
		vkw::GraphicsPipeline pipeline;
		vkw::ShaderModule vertexShader;
		vkw::ShaderModule fragmentShader;
	};

	Mesh teapot;
	Mesh teapot2;
	vkw::Memory uniformMemory;
	vkw::Buffer uniformBuffer;
	vkw::DescriptorPool descriptorPool;
	vkw::DescriptorSetLayout setLayout;
	vkw::DescriptorSet descriptorSet;
	vkw::PipelineLayout pipelineLayout;
	PipelineWithShaders phong;
	PipelineWithShaders wire;
	PipelineWithShaders toon;
};

	
PipelineExample::PipelineExample(Window & window) : ExampleBase(window) {
	auto func = [](const vkw::PhysicalDevice& phys, const vkw::Surface& surf) {
		return phys.features.fillModeNonSolid && phys.features.wideLines;
	};

	InitInfo initInfo = InitInfo();
	initInfo.deviceSuitableFkt.push_back(func);
	initInfo.deviceFeatures.fillModeNonSolid = VK_TRUE;
	initInfo.deviceFeatures.wideLines = VK_TRUE;
	initVulkan(initInfo);
	setup();
}

PipelineExample::~PipelineExample()
{
	vkDeviceWaitIdle(device);
	for (auto & x : drawCommandBuffers) x.destroyObject();

}

void PipelineExample::setup() {
	loadModels();
	createUBO();
	createDescriptorSets();
	createPipelines();
	setupRenderCommanBuffers();
}


void  PipelineExample::loadModels() {
	Mesh::LoadInfo meshInfo;
	meshInfo.pMesh = &teapot;
	meshInfo.layout = { VERTEX_COMPONENT_POSITION, VERTEX_COMPONENT_COLOR, VERTEX_COMPONENT_NORMAL };
	meshInfo.filePath = modelPath() + "Teapot.obj";

	Mesh::LoadInfo meshInfo2 = meshInfo;
	meshInfo2.pMesh = &teapot2;
	meshInfo2.layout = { VERTEX_COMPONENT_POSITION, VERTEX_COMPONENT_COLOR };
	meshInfo2.center = glm::vec3(1, 1, 1);
	meshLoader.loadFromFile({ meshInfo, meshInfo2 });
}

void  PipelineExample::createUBO() {
	uniformBuffer.createBuffer(VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, sizeof(UBO));

	vkw::Memory::AllocInfo allocInfo = {};
	allocInfo.buffers = { uniformBuffer };
	allocInfo.memoryFlags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;

	uniformMemory.allocateMemory(allocInfo);
}

void  PipelineExample::createDescriptorSets() {
	vkw::DescriptorPool::CreateInfo2 poolCreateInfo = {};
	poolCreateInfo.uniformBufferCount = 1;
	poolCreateInfo.maxSets = 2;
	descriptorPool.createDescriptorPool(poolCreateInfo);

	VkDescriptorSetLayoutBinding uboBinding = { 0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_VERTEX_BIT };
	setLayout.createDescriptorSetLayout({ uboBinding });

	pipelineLayout.createPipelineLayout({ setLayout }, {});

	vkw::DescriptorSet::AllocInfo allocInfo = {};
	allocInfo.descriptorPool = descriptorPool;
	allocInfo.layout = setLayout;
	descriptorSet.allocateDescriptorSet(allocInfo);

	VkDescriptorBufferInfo uboInfo = { uniformBuffer, 0, sizeof(UBO) };

	vkw::DescriptorSet::WriteInfo writeInfo = {};
	writeInfo.dstBinding = 0;
	writeInfo.descriptorCount = 1;
	writeInfo.pBufferInfo = &uboInfo;
	descriptorSet.update({ writeInfo }, {});
}

void  PipelineExample::createPipelines() {
	vkw::GraphicsPipeline::CreateInfo createInfo = {};


	VkPipelineInputAssemblyStateCreateInfo inputAssemblyState = vkw::init::pipelineInputAssemblyStateCreateInfo();
	inputAssemblyState.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	inputAssemblyState.primitiveRestartEnable = VK_FALSE;

	VkPipelineRasterizationStateCreateInfo rasterizationState = vkw::init::pipelineRasterizationStateCreateInfo();
	rasterizationState.polygonMode = VK_POLYGON_MODE_FILL;
	rasterizationState.cullMode = VK_CULL_MODE_FRONT_BIT;
	rasterizationState.frontFace = VK_FRONT_FACE_CLOCKWISE;

	VkPipelineColorBlendAttachmentState blendAttachmentState = vkw::init::pipelineColorBlendAttachmentState(0xf, VK_FALSE);

	VkPipelineColorBlendStateCreateInfo colorBlendState = vkw::init::pipelineColorBlendStateCreateInfo();
	colorBlendState.pAttachments = &blendAttachmentState;
	colorBlendState.attachmentCount = 1;

	VkPipelineDepthStencilStateCreateInfo depthStencilState = vkw::init::pipelineDepthStencilStateCreateInfo();
	depthStencilState.depthTestEnable = VK_TRUE;
	depthStencilState.depthWriteEnable = VK_TRUE;
	depthStencilState.depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;

	VkPipelineViewportStateCreateInfo viewportState = vkw::init::pipelineViewportStateCreateInfo();
	viewportState.scissorCount = 1;
	viewportState.viewportCount = 1;

	VkPipelineMultisampleStateCreateInfo multisampleState = vkw::init::pipelineMultisampleStateCreateInfo();
	multisampleState.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

	std::vector<VkDynamicState> dynamicStateEnables = {
		VK_DYNAMIC_STATE_VIEWPORT,
		VK_DYNAMIC_STATE_SCISSOR,
		VK_DYNAMIC_STATE_LINE_WIDTH,
	};

	VkPipelineDynamicStateCreateInfo dynamicState = vkw::init::pipelineDynamicStateCreateInfo();
	dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStateEnables.size());
	dynamicState.pDynamicStates = dynamicStateEnables.data();

	VkVertexInputBindingDescription bindingDescription = teapot.vertexBinding(0);
	std::vector<VkVertexInputAttributeDescription> vertexInputAttributes = teapot.vertexAttributes(0);

	VkPipelineVertexInputStateCreateInfo vertexInputState = vkw::init::pipelineVertexInputStateCreateInfo();
	vertexInputState.pVertexAttributeDescriptions = vertexInputAttributes.data();
	vertexInputState.vertexAttributeDescriptionCount = static_cast<uint32_t>(vertexInputAttributes.size());
	vertexInputState.pVertexBindingDescriptions = &bindingDescription;
	vertexInputState.vertexBindingDescriptionCount = 1;


	createInfo.inputAssemblyState = &inputAssemblyState;
	createInfo.rasterizationState = &rasterizationState;
	createInfo.colorBlendState = &colorBlendState;
	createInfo.depthStencilState = &depthStencilState;
	createInfo.viewportState = &viewportState;
	createInfo.multisampleState = &multisampleState;
	createInfo.dynamicState = &dynamicState;
	createInfo.vertexInputState = &vertexInputState;
	createInfo.renderPass = renderPass;
	createInfo.subPass = 0;
	createInfo.layout = pipelineLayout;
	createInfo.cache = pipelineCache;
	createInfo.flags = VK_PIPELINE_CREATE_ALLOW_DERIVATIVES_BIT;


	phong.vertexShader.createShaderModule(shaderPath() + "phong.vert.spv", VK_SHADER_STAGE_VERTEX_BIT);
	phong.fragmentShader.createShaderModule(shaderPath() + "phong.frag.spv", VK_SHADER_STAGE_FRAGMENT_BIT);
	createInfo.shaderStages = { phong.vertexShader.shaderStageInfo(),  phong.fragmentShader.shaderStageInfo() };
	phong.pipeline.createPipeline(createInfo);

	createInfo.flags = VK_PIPELINE_CREATE_DERIVATIVE_BIT;
	createInfo.basePipelineHandle = phong.pipeline;

	toon.vertexShader.createShaderModule(shaderPath() + "toon.vert.spv", VK_SHADER_STAGE_VERTEX_BIT);
	toon.fragmentShader.createShaderModule(shaderPath() + "toon.frag.spv", VK_SHADER_STAGE_FRAGMENT_BIT);
	createInfo.shaderStages = { toon.vertexShader.shaderStageInfo(),  toon.fragmentShader.shaderStageInfo() };
	toon.pipeline.createPipeline(createInfo);


	rasterizationState.polygonMode = VK_POLYGON_MODE_LINE;
	rasterizationState.cullMode = VK_CULL_MODE_NONE;

	bindingDescription = teapot2.vertexBinding(0);
	vertexInputAttributes = teapot2.vertexAttributes(0);
	
	vertexInputState = vkw::init::pipelineVertexInputStateCreateInfo();
	vertexInputState.pVertexAttributeDescriptions = vertexInputAttributes.data();
	vertexInputState.vertexAttributeDescriptionCount = static_cast<uint32_t>(vertexInputAttributes.size());
	vertexInputState.pVertexBindingDescriptions = &bindingDescription;
	vertexInputState.vertexBindingDescriptionCount = 1;


	wire.vertexShader.createShaderModule(shaderPath() + "wire.vert.spv", VK_SHADER_STAGE_VERTEX_BIT);
	wire.fragmentShader.createShaderModule(shaderPath() + "wire.frag.spv", VK_SHADER_STAGE_FRAGMENT_BIT);
	createInfo.shaderStages = { wire.vertexShader.shaderStageInfo(),  wire.fragmentShader.shaderStageInfo() };
	wire.pipeline.createPipeline(createInfo);
}

void  PipelineExample::setupRenderCommanBuffers() {
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

			vkCmdBindIndexBuffer(drawCommandBuffers[i], teapot.indexBuffer, 0, VK_INDEX_TYPE_UINT32);

			VkBuffer vertexBuffers[] = { teapot.vertexBuffer, teapot2.vertexBuffer };
			VkDeviceSize offsets[] = { teapot.vertexBuffer.offset, teapot2.vertexBuffer.offset };
			vkCmdBindVertexBuffers(drawCommandBuffers[i], 0, 1, vertexBuffers, offsets);

				VkRect2D scissor = { { 0,0 }, swapChain.extent };
				vkCmdSetScissor(drawCommandBuffers[i], 0, 1, &scissor);

				VkViewport viewport = vkw::init::viewport(swapChain.extent);
				viewport.width = static_cast<float>(viewport.width) / 3;

				vkCmdBindDescriptorSets(drawCommandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, descriptorSet.get(), 0, nullptr);

				// phong
				viewport.x = 0;
				vkCmdSetViewport(drawCommandBuffers[i], 0, 1, &viewport);
				vkCmdBindPipeline(drawCommandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, phong.pipeline);
				vkCmdDrawIndexed(drawCommandBuffers[i], static_cast<uint32_t>(teapot.indexCount), 1, 0, 0, 0);
				
				// toon
				viewport.x = viewport.width;
				vkCmdSetViewport(drawCommandBuffers[i], 0, 1, &viewport);
				vkCmdBindPipeline(drawCommandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, toon.pipeline);
				vkCmdDrawIndexed(drawCommandBuffers[i], static_cast<uint32_t>(teapot.indexCount), 1, 0, 0, 0);

				// wire
				VkBuffer vertexBuffers2[] = { teapot2.vertexBuffer };
				VkDeviceSize offsets2[] = { teapot2.vertexBuffer.offset };
				vkCmdBindVertexBuffers(drawCommandBuffers[i], 0, 1, vertexBuffers2, offsets2);
				viewport.x = 2 * viewport.width;
				vkCmdSetLineWidth(drawCommandBuffers[i], 2.0f);
				vkCmdSetViewport(drawCommandBuffers[i], 0, 1, &viewport);
				vkCmdBindPipeline(drawCommandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, wire.pipeline);
				vkCmdDrawIndexed(drawCommandBuffers[i], static_cast<uint32_t>(teapot2.indexCount), 1, 0, 0, 0);

			vkCmdEndRenderPass(drawCommandBuffers[i]);

		drawCommandBuffers[i].endCommandBuffer();
	}
}


void PipelineExample::nextFrame() {
	static auto startTime = std::chrono::high_resolution_clock::now();

	auto currentTime = std::chrono::high_resolution_clock::now();
	float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();
	
	UBO ubo = {};
	ubo.model = glm::mat4(1.0);
	ubo.model = glm::scale(ubo.model, glm::vec3(0.3f));
	ubo.model = glm::translate(ubo.model, glm::vec3(0.0f, 0.0f, -0.4f));
	ubo.model = glm::rotate(ubo.model, time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	ubo.model = glm::rotate(ubo.model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	ubo.view = glm::lookAt(glm::vec3(2, 2, 2), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	ubo.proj = glm::perspective(glm::radians(45.0f), (swapChain.extent.width / 3) / (float)swapChain.extent.height, 0.1f, 10.0f); 
	ubo.proj[1][1] *= -1;
	ubo.light = glm::vec3(2.0, 2.0, 2.0) * glm::mat3(glm::rotate(glm::mat4(1), time * glm::radians(90.0f), glm::vec3(0, 0, 1)));

	uniformBuffer.write(&ubo, sizeof(ubo));

	renderFrame();
}


int main() {
	GlfwWindow window(1600, 800);

	PipelineExample example(window);

	while (!glfwWindowShouldClose(window)) {
		example.nextFrame();
		glfwPollEvents();
	}
}

