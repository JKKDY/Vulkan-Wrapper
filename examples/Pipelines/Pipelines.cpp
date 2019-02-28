#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "ExampleBase.h"

#include "Window.hpp"
#include "VkInitializers.hpp"

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

std::string dataPath = "../data/";
std::string modelPath = dataPath + "Models/";
std::string shaderPath = "shader/";

using namespace example;

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

	Mesh dragon;
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
		if (phys.features.fillModeNonSolid == VK_TRUE && phys.features.wideLines == VK_TRUE)
			return true;
		else
			return false;
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
	for (auto & x : renderCommandBuffers) x.destroyObject();

}

void PipelineExample::setup() {
	loadModels();
	createUBO();
	createDescriptorSets();
	createPipelines();
	setupRenderCommanBuffers();
}

void  PipelineExample::loadModels() {
	MeshLoader::CreateInfo meshLInfo = {};
	meshLInfo.defaultVertexBufferSize = 20000000; //20mb
	meshLInfo.defaultIndexBufferSize = 3000000; //3mb
	meshLInfo.stagingBufferSize = 40000000; //40mb
	meshloader.create(meshLInfo);

	MeshLoadInfo meshInfo;
	meshInfo.layout = { VERTEX_COMPONENT_POSITION, VERTEX_COMPONENT_COLOR, VERTEX_COMPONENT_NORMAL };
	dragon = meshloader.loadFromFile(modelPath + "Teapot.obj", meshInfo);
}

void  PipelineExample::createUBO() {
	uniformBuffer.createBuffer(VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, sizeof(UBO));

	vkw::Memory::AllocationInfo allocInfo = {};
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

	VkVertexInputBindingDescription bindingDescription = dragon.getBindingDescription(0);
	std::vector<VkVertexInputAttributeDescription> vertexInputAttributes = dragon.getInputDescriptions(0);

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


	phong.vertexShader.createShaderModule(shaderPath + "phong.vert.spv", VK_SHADER_STAGE_VERTEX_BIT);
	phong.fragmentShader.createShaderModule(shaderPath + "phong.frag.spv", VK_SHADER_STAGE_FRAGMENT_BIT);
	createInfo.shaderStages = { phong.vertexShader.pipelineShaderStageInfo(),  phong.fragmentShader.pipelineShaderStageInfo() };
	phong.pipeline.createPipeline(createInfo);

	createInfo.flags = VK_PIPELINE_CREATE_DERIVATIVE_BIT;
	createInfo.basePipelineHandle = phong.pipeline;

	toon.vertexShader.createShaderModule(shaderPath + "toon.vert.spv", VK_SHADER_STAGE_VERTEX_BIT);
	toon.fragmentShader.createShaderModule(shaderPath + "toon.frag.spv", VK_SHADER_STAGE_FRAGMENT_BIT);
	createInfo.shaderStages = { toon.vertexShader.pipelineShaderStageInfo(),  toon.fragmentShader.pipelineShaderStageInfo() };
	toon.pipeline.createPipeline(createInfo);

	rasterizationState.polygonMode = VK_POLYGON_MODE_LINE;

	wire.vertexShader.createShaderModule(shaderPath + "wire.vert.spv", VK_SHADER_STAGE_VERTEX_BIT);
	wire.fragmentShader.createShaderModule(shaderPath + "wire.frag.spv", VK_SHADER_STAGE_FRAGMENT_BIT);
	createInfo.shaderStages = { wire.vertexShader.pipelineShaderStageInfo(),  wire.fragmentShader.pipelineShaderStageInfo() };
	wire.pipeline.createPipeline(createInfo);
}

void  PipelineExample::setupRenderCommanBuffers() {
	renderCommandBuffers.resize(swapChain.imageCount);
	vkw::CommandBuffer::allocateCommandBuffers(renderCommandBuffers, graphicsCommandPool);

	for (uint32_t i = 0; i < renderCommandBuffers.size(); i++) {
		VkClearValue clearValues[2];
		clearValues[0] = { 0.0f, 0.0f, 0.0f, 1.0f };
		clearValues[1].depthStencil = { 1.0f , 0 };

		VkRenderPassBeginInfo beginnInfo = vkw::init::renderPassBeginInfo();
		beginnInfo.renderPass = renderPass;
		beginnInfo.renderArea = { { 0,0 }, swapChain.extent };
		beginnInfo.framebuffer = renderFrameBuffers[i];
		beginnInfo.clearValueCount = 2;
		beginnInfo.pClearValues = clearValues;

		renderCommandBuffers[i].beginCommandBuffer(VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT);

			vkCmdBeginRenderPass(renderCommandBuffers[i], &beginnInfo, VK_SUBPASS_CONTENTS_INLINE);

			vkCmdBindIndexBuffer(renderCommandBuffers[i], dragon.indexSubBuffer, 0, VK_INDEX_TYPE_UINT32);

			VkBuffer vertexBuffers[] = { dragon.vertexSubBuffer };
			VkDeviceSize offsets[] = { 0 };
			vkCmdBindVertexBuffers(renderCommandBuffers[i], 0, 1, vertexBuffers, offsets);

				VkRect2D scissor = { { 0,0 }, swapChain.extent };
				vkCmdSetScissor(renderCommandBuffers[i], 0, 1, &scissor);

				VkViewport viewport = vkw::init::viewport(swapChain.extent);
				viewport.width = static_cast<float>(viewport.width) / 3;

				vkCmdBindDescriptorSets(renderCommandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, descriptorSet.get(), 0, nullptr);

				// phong
				viewport.x = 0;
				vkCmdSetViewport(renderCommandBuffers[i], 0, 1, &viewport);
				vkCmdBindPipeline(renderCommandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, phong.pipeline);
				vkCmdDrawIndexed(renderCommandBuffers[i], static_cast<uint32_t>(dragon.indexCount), 1, 0, 0, 0);
				
				// toon
				viewport.x = viewport.width;
				vkCmdSetViewport(renderCommandBuffers[i], 0, 1, &viewport);
				vkCmdBindPipeline(renderCommandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, toon.pipeline);
				vkCmdDrawIndexed(renderCommandBuffers[i], static_cast<uint32_t>(dragon.indexCount), 1, 0, 0, 0);

				// wire
				viewport.x = 2 * viewport.width;
				vkCmdSetLineWidth(renderCommandBuffers[i], 2.0f);
				vkCmdSetViewport(renderCommandBuffers[i], 0, 1, &viewport);
				vkCmdBindPipeline(renderCommandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, wire.pipeline);
				vkCmdDrawIndexed(renderCommandBuffers[i], static_cast<uint32_t>(dragon.indexCount), 1, 0, 0, 0);

			vkCmdEndRenderPass(renderCommandBuffers[i]);

		renderCommandBuffers[i].endCommandBuffer();
	}
}


void PipelineExample::nextFrame() {
	static auto startTime = std::chrono::high_resolution_clock::now();

	auto currentTime = std::chrono::high_resolution_clock::now();
	float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();
	
	UBO ubo = {};
	ubo.model = glm::mat4(1.0);
	ubo.model = glm::scale(ubo.model, glm::vec3(0.3));
	ubo.model = glm::translate(ubo.model, glm::vec3(0.0f, 0.0f, -0.4f));
	ubo.model = glm::rotate(ubo.model, time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	ubo.model = glm::rotate(ubo.model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	ubo.view = glm::lookAt(glm::vec3(2, 2, 2), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	ubo.proj = glm::perspective(glm::radians(45.0f), (swapChain.extent.width / 3) / (float)swapChain.extent.height, 0.1f, 10.0f); 
	ubo.proj[1][1] *= -1;
	ubo.light = glm::vec3(2.0, 2.0, 2.0) * glm::mat3(glm::rotate(glm::mat4(1), time * glm::radians(90.0f), glm::vec3(0, 0, 1)));

	uniformBuffer.write(&ubo, sizeof(ubo));

	renderFrame();

	sleep(10);
}


int main() {
	GlfwWindow window(1600, 800);

	PipelineExample example(window);

	while (!glfwWindowShouldClose(window)) {
		example.nextFrame();
		glfwPollEvents();
	}
}

