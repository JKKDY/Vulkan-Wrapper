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
	void preparePipelines();
	void buildCommandBuffers();

	struct UBO {
		glm::mat4 projection;
		glm::mat4 model;
		glm::vec4 lightPos = glm::vec4(0.0, 0.0, -2.0, 1.0);
	} ubo;

	std::array<glm::vec4, 6> pushConstants;

	vkex::Mesh scene;

	vkw::Memory uniformBufferMemory;
	vkw::Buffer uniformBuffer;

	vkw::DescriptorSetLayout descriptorSetLayout;
	vkw::DescriptorPool descriptorPool;
	vkw::DescriptorSet descriptorSet;
	vkw::PipelineLayout pipelineLayout;
	vkw::GraphicsPipeline pipeline;
};

VkwExample::VkwExample(Window & window) : ExampleBase(window) {
	auto func = [&](const vkw::PhysicalDevice& phys, const vkw::Surface& surf) {
		return sizeof(pushConstants) < phys.properties.limits.maxPushConstantsSize;
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
	preparePipelines();
	buildCommandBuffers();
}


void VkwExample::loadAssets() {
	vkex::Mesh::LoadInfo meshInfo(modelPath() + "samplescene.dae", { {VERTEX_COMPONENT_POSITION, VERTEX_COMPONENT_NORMAL, VERTEX_COMPONENT_UV, VERTEX_COMPONENT_COLOR} }, &scene);
	meshInfo.scale = glm::vec3(0.35f);

	meshLoader.loadFromFile({ meshInfo });
}

void VkwExample::prepareUniformBuffers() {
	uniformBuffer.createBuffer(VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, sizeof(UBO));
	uniformBufferMemory.allocateMemory(VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, { uniformBuffer }, {});
}

void VkwExample::setupDescriptors() {
	VkDescriptorSetLayoutBinding uniformLayoutBinding = {};
	uniformLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	uniformLayoutBinding.binding = 0;
	uniformLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
	uniformLayoutBinding.descriptorCount = 1;

	descriptorSetLayout.createDescriptorSetLayout({ uniformLayoutBinding });

	vkw::DescriptorPool::CreateInfo2 descriptorPoolCreateInfo = {};
	descriptorPoolCreateInfo.uniformBufferCount = 1;
	descriptorPoolCreateInfo.maxSets = 2; //----------

	descriptorPool.createDescriptorPool(descriptorPoolCreateInfo);

	descriptorSet.allocateDescriptorSet(descriptorPool, descriptorSetLayout);

	VkDescriptorBufferInfo bufferInfo = uniformBuffer.bufferInfo();
	vkw::DescriptorSet::WriteInfo bufferWriteInfo = {};
	bufferWriteInfo.dstBinding = 0;
	bufferWriteInfo.descriptorCount = 1;
	bufferWriteInfo.pBufferInfo = &bufferInfo;

	descriptorSet.update({ bufferWriteInfo }, {});
}

void VkwExample::preparePipelines()
{
	VkPushConstantRange pushConstantRange = {};
	pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
	pushConstantRange.size = sizeof(pushConstants);
	pushConstantRange.offset = 0;
	pipelineLayout.createPipelineLayout({ descriptorSetLayout }, { pushConstantRange });


	const std::vector<VkDynamicState> dynamicStateEnables = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };

	VkPipelineInputAssemblyStateCreateInfo inputAssemblyState = initializers::pipelineInputAssemblyStateCreateInfo(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, VK_FALSE);
	VkPipelineRasterizationStateCreateInfo rasterizationState = initializers::pipelineRasterizationStateCreateInfo(VK_POLYGON_MODE_FILL, VK_CULL_MODE_BACK_BIT, VK_FRONT_FACE_CLOCKWISE);
	VkPipelineColorBlendAttachmentState blendAttachmentState = vkw::init::pipelineColorBlendAttachmentState(0xf, VK_FALSE);
	VkPipelineColorBlendStateCreateInfo colorBlendState = initializers::pipelineColorBlendStateCreateInfo(1, &blendAttachmentState);
	VkPipelineDepthStencilStateCreateInfo depthStencilState = initializers::pipelineDepthStencilStateCreateInfo(VK_TRUE, VK_TRUE, VK_COMPARE_OP_LESS_OR_EQUAL);
	VkPipelineViewportStateCreateInfo viewportState = initializers::pipelineViewportStateCreateInfo(1, 1);
	VkPipelineMultisampleStateCreateInfo multisampleState = initializers::pipelineMultisampleStateCreateInfo(VK_SAMPLE_COUNT_1_BIT);
	VkPipelineDynamicStateCreateInfo dynamicState = initializers::pipelineDynamicStateCreateInfo(dynamicStateEnables);

	const std::vector<VkVertexInputBindingDescription> vertexBindings = { scene.vertexBinding(0) };
	const std::vector<VkVertexInputAttributeDescription> vertexInputAttributes = scene.vertexAttributes(0);

	VkPipelineVertexInputStateCreateInfo vertexInputState = vkw::init::pipelineVertexInputStateCreateInfo();
	vertexInputState.vertexBindingDescriptionCount = static_cast<uint32_t>(vertexBindings.size());
	vertexInputState.pVertexBindingDescriptions = vertexBindings.data();
	vertexInputState.vertexAttributeDescriptionCount = static_cast<uint32_t>(vertexInputAttributes.size());
	vertexInputState.pVertexAttributeDescriptions = vertexInputAttributes.data();

	vkw::ShaderModule vertexShader(shaderPath() + "lights.vert.spv", VK_SHADER_STAGE_VERTEX_BIT);
	vkw::ShaderModule fragShader(shaderPath() + "lights.frag.spv", VK_SHADER_STAGE_FRAGMENT_BIT);

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

#define r 7.5f
#define sin_t sin(glm::radians(time * 360.f))
#define cos_t cos(glm::radians(time* 360.f))
#define y -4.0f
				pushConstants[0] = glm::vec4(r * 1.1 * sin_t, y, r * 1.1 * cos_t, 1.0f);
				pushConstants[1] = glm::vec4(-r * sin_t, y, -r * cos_t, 1.0f);
				pushConstants[2] = glm::vec4(r * 0.85f * sin_t, y, -sin_t * 2.5f, 1.5f);
				pushConstants[3] = glm::vec4(0.0f, y, r * 1.25f * cos_t, 1.5f);
				pushConstants[4] = glm::vec4(r * 2.25f * cos_t, y, 0.0f, 1.25f);
				pushConstants[5] = glm::vec4(r * 2.5f * cos_t, y, r * 2.5f * sin_t, 1.25f);
#undef r
#undef y
#undef sin_t
#undef cos_t

				vkCmdPushConstants(drawCommandBuffers[i], pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(pushConstants), pushConstants.data());

				VkDeviceSize offsets[] = { 0 };
				VkBuffer buffers[] = { scene.vertexBuffer };
				vkCmdBindVertexBuffers(drawCommandBuffers[i], 0, 1, buffers, offsets);
				vkCmdBindIndexBuffer(drawCommandBuffers[i], scene.indexBuffer, 0, VK_INDEX_TYPE_UINT32);

				vkCmdBindPipeline(drawCommandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
				vkCmdBindDescriptorSets(drawCommandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, descriptorSet.get(), 0, nullptr);

				vkCmdDrawIndexed(drawCommandBuffers[i], scene.indexCount, 1, 0, 0, 0);

			vkCmdEndRenderPass(drawCommandBuffers[i]);

		drawCommandBuffers[i].endCommandBuffer();
	}
}


void VkwExample::nextFrame() {
	glm::mat4 viewMatrix = glm::mat4(1.0f);
	ubo.projection = glm::perspective(glm::radians(60.0f), (float)swapChain.extent.width / (float)swapChain.extent.height, 0.001f, 256.0f);
	viewMatrix = glm::translate(viewMatrix, glm::vec3(0.0f, 2.0f, -30.0f));

	ubo.model = viewMatrix * glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0));
	ubo.model = glm::rotate(ubo.model, glm::radians(-32.5f), glm::vec3(1.0f, 0.0f, 0.0f));
	ubo.model = glm::rotate(ubo.model, glm::radians(45.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	ubo.model = glm::rotate(ubo.model, glm::radians(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));

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

