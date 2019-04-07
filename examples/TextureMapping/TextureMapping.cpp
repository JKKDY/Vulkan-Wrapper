#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <gli/gli.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

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
	void loadTexture();
	void generatePlane();
	void setupDescriptors();
	void createPipelines();
	void buildCommandBuffers();

	struct Vertex {
		float pos[2];
		float uv[2];
	};

	const bool loadWithSTB = true;

	vkw::Memory textureMemory;
	vkw::Image texture;
	vkw::ImageView imageView;
	vkw::Sampler sampler;
	
	vkw::Memory meshMemory;
	vkw::Buffer vertexBuffer;
	vkw::Buffer indexBuffer;
	uint32_t indexCount;

	vkw::DescriptorSetLayout descriptorSetLayout;
	vkw::DescriptorPool descriptorPool;
	vkw::DescriptorSet descriptorSet;

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
	loadTexture();
	generatePlane();
	setupDescriptors();
	createPipelines();
	buildCommandBuffers();
}

void VkwExample::loadTexture()
{	
	vkw::Memory stagingMemory(VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
	vkw::Buffer stagingBuffer;


	vkw::Image::CreateInfo imageCreateInfo = {};
	imageCreateInfo.format = VK_FORMAT_R8G8B8A8_UNORM;
	imageCreateInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
	imageCreateInfo.arrayLayers = 1;
	imageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
	imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	imageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
	imageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
	imageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	imageCreateInfo.familyQueueIndicies = {};
	imageCreateInfo.flags = 0;

	if (loadWithSTB) {
		int texWidth, texHeight, texChannels;
		stbi_uc* pixels = stbi_load((texturePath() + "vector_art_sunset_jet.png").c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);

		imageCreateInfo.extent = { (uint32_t)texWidth , (uint32_t)texHeight, 1 };
		imageCreateInfo.mipLevels = 1;

		if (!pixels) { throw std::runtime_error("failed to load texture image!");}
		
		stagingBuffer.createBuffer(VK_BUFFER_USAGE_TRANSFER_SRC_BIT, texWidth * texHeight * 4);
		stagingMemory.allocateMemory({ stagingBuffer });
		stagingBuffer.write(pixels, texWidth * texHeight * 4);
		stbi_image_free(pixels);
	}
	else {
		gli::texture2d tex2D(gli::load((texturePath() + "metalplate01_rgba.ktx").c_str()));

		imageCreateInfo.extent = { static_cast<uint32_t>(tex2D.extent().x), static_cast<uint32_t>(tex2D.extent().y), 1 };
		imageCreateInfo.mipLevels = static_cast<uint32_t>(tex2D.levels());

		stagingBuffer.createBuffer(VK_BUFFER_USAGE_TRANSFER_SRC_BIT, tex2D.size());
		stagingMemory.allocateMemory({ stagingBuffer });
		stagingBuffer.write(tex2D.data(), tex2D.size());
	}

	texture.createImage(imageCreateInfo);
	textureMemory.allocateMemory(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, {}, { texture });

	texture.transitionImageLayout(VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_ASPECT_COLOR_BIT);

	VkBufferImageCopy bufferCopyRegion = {};
	bufferCopyRegion.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	bufferCopyRegion.imageSubresource.mipLevel = 0;
	bufferCopyRegion.imageSubresource.baseArrayLayer = 0;
	bufferCopyRegion.imageSubresource.layerCount = 1;
	bufferCopyRegion.imageExtent.width = imageCreateInfo.extent.width;
	bufferCopyRegion.imageExtent.height = imageCreateInfo.extent.height;
	bufferCopyRegion.imageExtent.depth = 1;
	bufferCopyRegion.bufferOffset = 0;

	texture.copyFromBuffer(stagingBuffer, { bufferCopyRegion });

	texture.transitionImageLayout(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VK_IMAGE_ASPECT_COLOR_BIT);


	vkw::Sampler::CreateInfo samplerCreateInfo = {};
	samplerCreateInfo.magFilter = VK_FILTER_LINEAR;
	samplerCreateInfo.minFilter = VK_FILTER_LINEAR;
	samplerCreateInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
	samplerCreateInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerCreateInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerCreateInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerCreateInfo.mipLodBias = 0.0f;
	samplerCreateInfo.compareEnable = VK_FALSE;
	samplerCreateInfo.compareOp = VK_COMPARE_OP_NEVER;
	samplerCreateInfo.minLod = 0.0f;
	samplerCreateInfo.maxLod = 0.0f;
	samplerCreateInfo.maxAnisotropy = physicalDevice.properties.limits.maxSamplerAnisotropy;
	samplerCreateInfo.anisotropyEnable = VK_FALSE;
	samplerCreateInfo.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;
	sampler.createSampler(samplerCreateInfo);

	vkw::ImageView::CreateInfo viewCreateInfo = {};
	viewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
	viewCreateInfo.components = { VK_COMPONENT_SWIZZLE_R, VK_COMPONENT_SWIZZLE_G, VK_COMPONENT_SWIZZLE_B, VK_COMPONENT_SWIZZLE_A };
	viewCreateInfo.subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };
	viewCreateInfo.subresourceRange.levelCount = texture.mipLevels;
	viewCreateInfo.image = texture;
	imageView.createImageView(viewCreateInfo);
}

void VkwExample::generatePlane()
{
	const std::vector<Vertex> vertices = {
		{ {-0.5f, -0.5f}, {1.0f, 0.0f} },
		{ {0.5f, -0.5f},  {0.0f, 0.0f} },
		{ {0.5f, 0.5f}, {0.0f, 1.0f} },
		{ {-0.5f, 0.5f}, {1.0f, 1.0f} }
	};

	const std::vector<uint32_t> indices = {
		0, 1, 2, 2, 3, 0
	};

	indexCount = static_cast<uint32_t>(indices.size());

	vkw::Memory stagingMemory(VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
	vkw::Buffer vertexStagingBuffer(VK_BUFFER_USAGE_TRANSFER_SRC_BIT, vertices.size() * sizeof(Vertex));
	vkw::Buffer	indexStagingBuffer(VK_BUFFER_USAGE_TRANSFER_SRC_BIT, indices.size() * sizeof(uint32_t));
	stagingMemory.allocateMemory({ vertexStagingBuffer, indexStagingBuffer });

	vertexBuffer.createBuffer(VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, vertices.size() * sizeof(Vertex));
	indexBuffer.createBuffer(VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, indices.size() * sizeof(uint32_t));
	meshMemory.allocateMemory(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, { vertexBuffer, indexBuffer });

	vertexStagingBuffer.write(vertices.data(), vertices.size() * sizeof(Vertex));
	indexStagingBuffer.write(indices.data(), indices.size() * sizeof(uint32_t));

	vertexBuffer.copyFromBuffer(vertexStagingBuffer);
	indexBuffer.copyFromBuffer(indexStagingBuffer);
}

void VkwExample::setupDescriptors() {
	VkDescriptorSetLayoutBinding samplerLayoutBinding = {};
	samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	samplerLayoutBinding.binding = 0;
	samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
	samplerLayoutBinding.descriptorCount = 1;

	descriptorSetLayout.createDescriptorSetLayout({ samplerLayoutBinding });


	vkw::DescriptorPool::CreateInfo2 descriptorPoolCreateInfo = {};
	descriptorPoolCreateInfo.combinedImageSamplerCount = 1;
	descriptorPoolCreateInfo.maxSets = 1;
	descriptorPool.createDescriptorPool(descriptorPoolCreateInfo);


	descriptorSet.allocateDescriptorSet(descriptorPool, descriptorSetLayout);

	VkDescriptorImageInfo imageInfo = {};
	imageInfo.imageLayout = texture.layout;
	imageInfo.imageView = imageView;
	imageInfo.sampler = sampler;

	vkw::DescriptorSet::WriteInfo imageWriteInfo = {};
	imageWriteInfo.dstBinding = 0;
	imageWriteInfo.descriptorCount = 1;
	imageWriteInfo.pImageInfo = &imageInfo;

	descriptorSet.update({ imageWriteInfo }, {});
}

void VkwExample::createPipelines()
{
	pipelineLayout.createPipelineLayout({ descriptorSetLayout }, {});

	VkPipelineInputAssemblyStateCreateInfo inputAssemblyState = initializers::pipelineInputAssemblyStateCreateInfo(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, VK_FALSE);
	VkPipelineRasterizationStateCreateInfo rasterizationState = initializers::pipelineRasterizationStateCreateInfo(VK_POLYGON_MODE_FILL, VK_CULL_MODE_NONE, VK_FRONT_FACE_CLOCKWISE);
	VkPipelineColorBlendAttachmentState blendAttachmentState = vkw::init::pipelineColorBlendAttachmentState(0xf, VK_FALSE);
	VkPipelineColorBlendStateCreateInfo colorBlendState = initializers::pipelineColorBlendStateCreateInfo(1, &blendAttachmentState);
	VkPipelineDepthStencilStateCreateInfo depthStencilState = initializers::pipelineDepthStencilStateCreateInfo(VK_TRUE, VK_TRUE, VK_COMPARE_OP_LESS_OR_EQUAL);
	VkPipelineMultisampleStateCreateInfo multisampleState = initializers::pipelineMultisampleStateCreateInfo(VK_SAMPLE_COUNT_1_BIT);
	VkPipelineDynamicStateCreateInfo dynamicState = initializers::pipelineDynamicStateCreateInfo({});

	std::vector<VkVertexInputBindingDescription> vertexBindings(1);
	vertexBindings[0].binding = 0;
	vertexBindings[0].stride = sizeof(Vertex);
	vertexBindings[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

	std::vector<VkVertexInputAttributeDescription> vertexInputAttributes(2);
	vertexInputAttributes[0].binding = 0;
	vertexInputAttributes[0].location = 0;
	vertexInputAttributes[0].format = VK_FORMAT_R32G32_SFLOAT;
	vertexInputAttributes[0].offset = offsetof(Vertex, pos);

	vertexInputAttributes[1].binding = 0;
	vertexInputAttributes[1].location = 1;
	vertexInputAttributes[1].format = VK_FORMAT_R32G32_SFLOAT;
	vertexInputAttributes[1].offset = offsetof(Vertex, uv);

	VkPipelineVertexInputStateCreateInfo vertexInputState = vkw::init::pipelineVertexInputStateCreateInfo();
	vertexInputState.vertexBindingDescriptionCount = static_cast<uint32_t>(vertexBindings.size());
	vertexInputState.pVertexBindingDescriptions = vertexBindings.data();
	vertexInputState.vertexAttributeDescriptionCount = static_cast<uint32_t>(vertexInputAttributes.size());
	vertexInputState.pVertexAttributeDescriptions = vertexInputAttributes.data();

	VkRect2D scissor = vkw::init::rect2D(swapChain.extent);
	VkViewport viewport = vkw::init::viewport(swapChain.extent);

	VkPipelineViewportStateCreateInfo viewportState = vkw::init::pipelineViewportStateCreateInfo();
	viewportState.scissorCount = 1;
	viewportState.pScissors = &scissor;
	viewportState.viewportCount = 1;
	viewportState.pViewports = &viewport;

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

				VkDeviceSize offsets[] = { 0 };
				VkBuffer buffers[] = { vertexBuffer };
				vkCmdBindVertexBuffers(drawCommandBuffers[i], 0, 1, buffers, offsets);
				vkCmdBindIndexBuffer(drawCommandBuffers[i], indexBuffer, 0, VK_INDEX_TYPE_UINT32);
				
				vkCmdBindPipeline(drawCommandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
				vkCmdBindDescriptorSets(drawCommandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, descriptorSet.get(), 0, nullptr);
				vkCmdDrawIndexed(drawCommandBuffers[i], indexCount, 1, 0, 0, 0);

			vkCmdEndRenderPass(drawCommandBuffers[i]);

		drawCommandBuffers[i].endCommandBuffer();
	}
}

void VkwExample::nextFrame() {
	renderFrame();
}


int main() {
	GlfwWindow window(1200, 800);

	auto example = VkwExample(window);
	while (!glfwWindowShouldClose(window)) {
		example.nextFrame();
		glfwPollEvents();
	}
}

