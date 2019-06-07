#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "ExampleBase.h"

#include "Window.hpp"
#include "VkInitializers.hpp"


using namespace vkx;

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
		glm::mat4 projection;
		glm::mat4 modelView;
		glm::vec4 lightPos = glm::vec4(0.0f, -2.0f, 1.0f, 0.0f);
	} ubo;

	Mesh cube;
	Texture2D texture;

	vkw::Memory uniformBufferMemory;
	vkw::Buffer uniformBuffer;

	vkw::DescriptorSetLayout descriptorSetLayout;
	vkw::DescriptorPool descriptorPool;
	vkw::DescriptorSet descriptorSet;

	vkw::PipelineLayout pipelineLayout;
	
	struct {
		vkw::GraphicsPipeline toon;
		vkw::GraphicsPipeline phong;
		vkw::GraphicsPipeline textured;
	}pipelines;
};

VkwExample::VkwExample(Window & window) : ExampleBase(window) {
	auto func = [](const vkw::PhysicalDevice& phys, const vkw::Surface& surf) {
		return phys.features.fillModeNonSolid && phys.features.wideLines;
	};

	InitInfo initInfo = InitInfo();
	initInfo.deviceSuitableFkt.push_back(func);
	initVulkan(initInfo);

	camera.setPerspective(60.0f, (swapChain.extent.width / 3.0f) / swapChain.extent.height, 0.1f, 512.0f);
	camera.translate({ 0,0,-2 });
	camera.rotate({ -40, -90, 0 });

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

void VkwExample::loadAssets(){
	Mesh::LoadInfo meshInfo(modelPath() + "color_teapot_spheres.dae", { {VERTEX_COMPONENT_POSITION, VERTEX_COMPONENT_NORMAL, VERTEX_COMPONENT_UV, VERTEX_COMPONENT_COLOR} }, &cube);
	meshInfo.scale = glm::vec3(0.1);
	meshLoader.loadFromFile({ meshInfo });

	texture.loadFromFile(texturePath() + "metalplate_nomips_rgba.ktx", VK_FORMAT_R8G8B8A8_UNORM);
}

void VkwExample::prepareUniformBuffers()
{
	uniformBuffer.createBuffer(VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, sizeof(UBO));
	uniformBufferMemory.allocateMemory(VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, { uniformBuffer }, {});

	ubo.modelView = camera.view;
	ubo.projection = camera.perspective;

	uniformBuffer.write(&ubo, sizeof(UBO));
}

void VkwExample::setupDescriptors(){
	const std::vector<VkDescriptorSetLayoutBinding> setLayoutBindings = {
			vkw::init::descriptorSetLayoutBinding(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT, 0),
			vkw::init::descriptorSetLayoutBinding(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT, 1),
	};

	descriptorSetLayout.createDescriptorSetLayout(setLayoutBindings);

	vkw::DescriptorPool::CreateInfo2 poolCreateInfo = {};
	poolCreateInfo.uniformBufferCount = 1;
	poolCreateInfo.combinedImageSamplerCount = 1;
	poolCreateInfo.maxSets = 1;
	descriptorPool.createDescriptorPool(poolCreateInfo);

	descriptorSet.allocateDescriptorSet(descriptorPool, descriptorSetLayout);

	VkDescriptorBufferInfo bufferInfo = uniformBuffer.bufferInfo();
	VkDescriptorImageInfo imageInfo = texture.descriptorInfo();

	const std::vector<vkw::DescriptorSet::WriteInfo> writeDescriptorSets = {
			initializers::descriptorSetWriteInfo(0, &bufferInfo),
			initializers::descriptorSetWriteInfo(1, &imageInfo),
	};

	descriptorSet.update(writeDescriptorSets, {});
}

void VkwExample::createPipelines()
{
	pipelineLayout.createPipelineLayout({ descriptorSetLayout }, {});

	const std::vector<VkDynamicState> dynamicStateEnables = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR, VK_DYNAMIC_STATE_LINE_WIDTH };

	VkPipelineInputAssemblyStateCreateInfo inputAssemblyState = initializers::pipelineInputAssemblyStateCreateInfo(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, VK_FALSE);
	VkPipelineRasterizationStateCreateInfo rasterizationState = initializers::pipelineRasterizationStateCreateInfo(VK_POLYGON_MODE_FILL, VK_CULL_MODE_NONE, VK_FRONT_FACE_CLOCKWISE);
	VkPipelineColorBlendAttachmentState blendAttachmentState = vkw::init::pipelineColorBlendAttachmentState(0xf, VK_FALSE);
	VkPipelineViewportStateCreateInfo viewportState = initializers::pipelineViewportStateCreateInfo(1, 1);
	VkPipelineColorBlendStateCreateInfo colorBlendState = initializers::pipelineColorBlendStateCreateInfo(1, &blendAttachmentState);
	VkPipelineDepthStencilStateCreateInfo depthStencilState = initializers::pipelineDepthStencilStateCreateInfo(VK_TRUE, VK_TRUE, VK_COMPARE_OP_LESS_OR_EQUAL);
	VkPipelineMultisampleStateCreateInfo multisampleState = initializers::pipelineMultisampleStateCreateInfo(VK_SAMPLE_COUNT_1_BIT);
	VkPipelineDynamicStateCreateInfo dynamicState = initializers::pipelineDynamicStateCreateInfo(dynamicStateEnables);
	
	const std::vector<VkVertexInputBindingDescription> vertexBindings = { cube.vertexBinding(0) };
	const std::vector<VkVertexInputAttributeDescription> vertexInputAttributes = cube.vertexAttributes(0);

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

	struct SpecializationData {
		uint32_t lightingModel;
		float toonDesaturationFactor = 0.5f;
	} specializationData;

	std::array<VkSpecializationMapEntry, 2> specializationMapEntries;
	specializationMapEntries[0].constantID = 0;
	specializationMapEntries[0].size = sizeof(specializationData.lightingModel);
	specializationMapEntries[0].offset = 0;

	specializationMapEntries[1].constantID = 1;
	specializationMapEntries[1].size = sizeof(specializationData.toonDesaturationFactor);
	specializationMapEntries[1].offset = offsetof(SpecializationData, toonDesaturationFactor);

	VkSpecializationInfo specializationInfo = {};
	specializationInfo.dataSize = sizeof(specializationData);
	specializationInfo.mapEntryCount = static_cast<uint32_t>(specializationMapEntries.size());
	specializationInfo.pMapEntries = specializationMapEntries.data();
	specializationInfo.pData = &specializationData;

	vkw::ShaderModule vertexShader(shaderPath() + "uber.vert.spv", VK_SHADER_STAGE_VERTEX_BIT);
	vkw::ShaderModule fragShader(shaderPath() + "uber.frag.spv", VK_SHADER_STAGE_FRAGMENT_BIT);
	pipelineCreateInfo.shaderStages = { vertexShader.shaderStageInfo(), fragShader.shaderStageInfo(&specializationInfo) };

	specializationData.lightingModel = 0;
	pipelines.phong.createPipeline(pipelineCreateInfo);

	specializationData.lightingModel = 1;
	pipelines.toon.createPipeline(pipelineCreateInfo);

	specializationData.lightingModel = 2;
	pipelines.textured.createPipeline(pipelineCreateInfo);
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
				VkBuffer buffers[] = { cube.vertexBuffer };
				vkCmdBindVertexBuffers(drawCommandBuffers[i], 0, 1, buffers, offsets);
				vkCmdBindIndexBuffer(drawCommandBuffers[i], cube.indexBuffer, 0, VK_INDEX_TYPE_UINT32);

				vkCmdBindDescriptorSets(drawCommandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, descriptorSet.getPtr(), 0, nullptr);

				// Left
				viewport.width = (float)swapChain.extent.width / 3.0f;
				vkCmdSetViewport(drawCommandBuffers[i], 0, 1, &viewport);
				vkCmdBindPipeline(drawCommandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, pipelines.phong);
				vkCmdDrawIndexed(drawCommandBuffers[i], cube.indexCount, 1, 0, 0, 0);

				// Center
				viewport.x = (float)swapChain.extent.width / 3.0f;
				vkCmdSetViewport(drawCommandBuffers[i], 0, 1, &viewport);
				vkCmdBindPipeline(drawCommandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, pipelines.toon);
				vkCmdDrawIndexed(drawCommandBuffers[i], cube.indexCount, 1, 0, 0, 0);

				// Right
				viewport.x = (float)swapChain.extent.width / 3.0f + (float)swapChain.extent.width / 3.0f;
				vkCmdSetViewport(drawCommandBuffers[i], 0, 1, &viewport);
				vkCmdBindPipeline(drawCommandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, pipelines.textured);
				vkCmdDrawIndexed(drawCommandBuffers[i], cube.indexCount, 1, 0, 0, 0);

			vkCmdEndRenderPass(drawCommandBuffers[i]);

		drawCommandBuffers[i].endCommandBuffer();
	}
}


void VkwExample::nextFrame() {
	static auto startTime = std::chrono::high_resolution_clock::now();

	auto currentTime = std::chrono::high_resolution_clock::now();
	float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

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

