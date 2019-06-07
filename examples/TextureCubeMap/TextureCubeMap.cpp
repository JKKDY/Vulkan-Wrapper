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
	void loadTextureCube();
	void prepareUniformBuffers();
	void setupDescriptors();
	void preparePipelines();
	void buildCommandBuffers();

	struct UBO {
		glm::mat4 projection;
		glm::mat4 model;
		float lodBias = 0.0f;
	};

	Mesh skybox;
	Mesh sphere;

	vkw::Image texCubeImage;
	vkw::Memory texCubeMemory;
	vkw::Sampler texCubeSampler;
	vkw::ImageView texCubeView;

	vkw::Buffer sphereUniformBuffer;
	vkw::Buffer skyboxUniformBuffer;
	vkw::Memory uniformBufferMemory;

	vkw::DescriptorSetLayout descriptorSetLayout;
	vkw::DescriptorPool descriptorPool;
	vkw::DescriptorSet skyboxDescriptorSet;
	vkw::DescriptorSet sphereDescriptorSet;

	vkw::PipelineLayout pipelineLayout;
	vkw::GraphicsPipeline skyboxPipeline;
	vkw::GraphicsPipeline reflectPipeline;
};

VkwExample::VkwExample(Window & window) : ExampleBase(window) {
	auto func = [](const vkw::PhysicalDevice& phys, const vkw::Surface& surf) {
		return phys.features.samplerAnisotropy && (phys.features.textureCompressionBC || phys.features.textureCompressionASTC_LDR || phys.features.textureCompressionETC2);
	};

	auto scoreFunc = [](const vkw::PhysicalDevice& phys, const vkw::Surface& surf) {
		int score = 0;
		score += (phys.features.textureCompressionBC + phys.features.textureCompressionASTC_LDR + phys.features.textureCompressionETC2) * 1000;
		return score;
	};

	InitInfo initInfo = InitInfo();
	initInfo.rateDevicefkt.push_back(scoreFunc);
	initInfo.deviceSuitableFkt.push_back(func);
	initInfo.deviceFeatures.samplerAnisotropy = VK_TRUE;
	initInfo.deviceFeatures.textureCompressionBC = VK_TRUE;
	initInfo.deviceFeatures.textureCompressionASTC_LDR = VK_TRUE;
	initInfo.deviceFeatures.textureCompressionETC2 = VK_TRUE;
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
	Mesh::LoadInfo skyboxInfo(modelPath() + "cube.obj", { {VERTEX_COMPONENT_POSITION, VERTEX_COMPONENT_NORMAL, VERTEX_COMPONENT_UV} }, &skybox);
	Mesh::LoadInfo sphereInfo(modelPath() + "sphere.obj", { {VERTEX_COMPONENT_POSITION, VERTEX_COMPONENT_NORMAL, VERTEX_COMPONENT_UV} }, &sphere);
	sphereInfo.scale = { 0.05f };
	meshLoader.loadFromFile({ skyboxInfo,  sphereInfo });

	loadTextureCube();
}

void VkwExample::loadTextureCube()
{
	// Vulkan core supports three different compressed texture formats
	// As the support differs between implemementations we need to check device features and select a proper format and file
	std::string filename;
	VkFormat format;
	if (physicalDevice.features.textureCompressionBC) {
		filename = "cubemap_yokohama_bc3_unorm.ktx";
		format = VK_FORMAT_BC2_UNORM_BLOCK;
	}
	else if (physicalDevice.features.textureCompressionASTC_LDR) {
		filename = "cubemap_yokohama_astc_8x8_unorm.ktx";
		format = VK_FORMAT_ASTC_8x8_UNORM_BLOCK;
	}
	else if (physicalDevice.features.textureCompressionETC2) {
		filename = "cubemap_yokohama_etc2_unorm.ktx";
		format = VK_FORMAT_ETC2_R8G8B8_UNORM_BLOCK;
	}

	gli::texture_cube texCube(gli::load(texturePath() + "cubemap_yokohama_bc3_unorm.ktx"));

	std::vector<VkBufferImageCopy> bufferCopyRegions;
	size_t offset = 0;

	for (uint32_t face = 0; face < 6; face++) {
		for (uint32_t level = 0; level < texCube.levels(); level++) {
			VkBufferImageCopy bufferCopyRegion = {};
			bufferCopyRegion.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			bufferCopyRegion.imageSubresource.mipLevel = level;
			bufferCopyRegion.imageSubresource.baseArrayLayer = face;
			bufferCopyRegion.imageSubresource.layerCount = 1;
			bufferCopyRegion.imageExtent.width = static_cast<uint32_t>(texCube[face][level].extent().x);
			bufferCopyRegion.imageExtent.height = static_cast<uint32_t>(texCube[face][level].extent().y);
			bufferCopyRegion.imageExtent.depth = 1;
			bufferCopyRegion.bufferOffset = offset;

			bufferCopyRegions.push_back(bufferCopyRegion);

			offset += texCube[face][level].size();
		}
	}

	vkw::Image::CreateInfo imageCreateInfo;
	imageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
	imageCreateInfo.format = format;
	imageCreateInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
	imageCreateInfo.extent = { static_cast<uint32_t>(texCube.extent().x) , static_cast<uint32_t>(texCube.extent().y), 1 };
	imageCreateInfo.mipLevels = static_cast<uint32_t>(texCube.levels());
	imageCreateInfo.flags = VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT;  
	imageCreateInfo.arrayLayers = 6;	// Cube faces count as array layers in Vulkan

	texCubeImage.createImage(imageCreateInfo);
	texCubeImage.transitionImageLayout(VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_ASPECT_COLOR_BIT);
	texCubeMemory.allocateMemory(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, {}, { texCubeImage });

	vkw::Memory stagingMemory(VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
	vkw::Buffer stagingBuffer(VK_BUFFER_USAGE_TRANSFER_SRC_BIT, texCubeImage.sizeInMemory);
	stagingMemory.allocateMemory({ stagingBuffer });
	stagingBuffer.write(texCube.data(), texCube.size());

	texCubeImage.copyFromBuffer(stagingBuffer, bufferCopyRegions);
	texCubeImage.transitionImageLayout(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VK_IMAGE_ASPECT_COLOR_BIT);


	vkw::Sampler::CreateInfo samplerCreateInfo = {};
	samplerCreateInfo.magFilter = VK_FILTER_LINEAR;
	samplerCreateInfo.minFilter = VK_FILTER_LINEAR;
	samplerCreateInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
	samplerCreateInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerCreateInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerCreateInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerCreateInfo.mipLodBias = 0.0f;
	samplerCreateInfo.compareOp = VK_COMPARE_OP_NEVER;
	samplerCreateInfo.minLod = 0.0f;
	samplerCreateInfo.maxLod = (float)texCubeImage.mipLevels;
	samplerCreateInfo.maxAnisotropy = 16;
	samplerCreateInfo.anisotropyEnable = VK_TRUE;
	samplerCreateInfo.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;
	texCubeSampler.createSampler(samplerCreateInfo);


	vkw::ImageView::CreateInfo viewCreateInfo = {};
	viewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_CUBE;
	viewCreateInfo.components = { VK_COMPONENT_SWIZZLE_R, VK_COMPONENT_SWIZZLE_G, VK_COMPONENT_SWIZZLE_B, VK_COMPONENT_SWIZZLE_A };
	viewCreateInfo.subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };
	viewCreateInfo.subresourceRange.levelCount = texCubeImage.mipLevels;
	viewCreateInfo.subresourceRange.layerCount = texCubeImage.arrayLayers; // 6
	viewCreateInfo.image = texCubeImage;
	texCubeView.createImageView(viewCreateInfo);


}

void VkwExample::prepareUniformBuffers()
{
	skyboxUniformBuffer.createBuffer(VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, sizeof(UBO));
	sphereUniformBuffer.createBuffer(VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, sizeof(UBO));
	uniformBufferMemory.allocateMemory(VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, {skyboxUniformBuffer, sphereUniformBuffer});
}

void VkwExample::setupDescriptors() {
	// layout
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

	descriptorSetLayout.createDescriptorSetLayout({ uniformLayoutBinding, samplerLayoutBinding });

	// pool
	vkw::DescriptorPool::CreateInfo2 descriptorPoolCreateInfo = {};
	descriptorPoolCreateInfo.uniformBufferCount = 2;
	descriptorPoolCreateInfo.combinedImageSamplerCount = 2;
	descriptorPoolCreateInfo.maxSets = 2;

	descriptorPool.createDescriptorPool(descriptorPoolCreateInfo);

	// descriptor sets
	skyboxDescriptorSet.allocateDescriptorSet(descriptorPool, descriptorSetLayout);
	sphereDescriptorSet.allocateDescriptorSet(descriptorPool, descriptorSetLayout);


	VkDescriptorImageInfo textureDescriptor = {};
	textureDescriptor.sampler = texCubeSampler;
	textureDescriptor.imageLayout = texCubeImage.layout;
	textureDescriptor.imageView = texCubeView;


	VkDescriptorBufferInfo sphereBufferInfo = sphereUniformBuffer.bufferInfo();
	vkw::DescriptorSet::WriteInfo writeInfo1 = {};
	writeInfo1.dstBinding = 0;
	writeInfo1.pBufferInfo = &sphereBufferInfo;

	vkw::DescriptorSet::WriteInfo writeInfo2 = {};
	writeInfo2.dstBinding = 1;
	writeInfo2.pImageInfo = &textureDescriptor;
	
	sphereDescriptorSet.update({ writeInfo1, writeInfo2 }, {});


	VkDescriptorBufferInfo skyboxBufferInfo = skyboxUniformBuffer.bufferInfo();
	writeInfo1 = {};
	writeInfo1.dstBinding = 0;
	writeInfo1.pBufferInfo = &skyboxBufferInfo;

	skyboxDescriptorSet.update({ writeInfo1, writeInfo2 }, {});
}

void VkwExample::preparePipelines() {

	pipelineLayout.createPipelineLayout({descriptorSetLayout}, {});

	const std::vector<VkDynamicState> dynamicStateEnables = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };

	VkPipelineInputAssemblyStateCreateInfo inputAssemblyState = initializers::pipelineInputAssemblyStateCreateInfo(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, VK_FALSE);
	VkPipelineRasterizationStateCreateInfo rasterizationState = initializers::pipelineRasterizationStateCreateInfo(VK_POLYGON_MODE_FILL, VK_CULL_MODE_BACK_BIT, VK_FRONT_FACE_COUNTER_CLOCKWISE);
	VkPipelineColorBlendAttachmentState blendAttachmentState = vkw::init::pipelineColorBlendAttachmentState(0xf, VK_FALSE);
	VkPipelineColorBlendStateCreateInfo colorBlendState = initializers::pipelineColorBlendStateCreateInfo(1, &blendAttachmentState);
	VkPipelineDepthStencilStateCreateInfo depthStencilState = initializers::pipelineDepthStencilStateCreateInfo(VK_FALSE, VK_FALSE, VK_COMPARE_OP_LESS_OR_EQUAL);
	VkPipelineViewportStateCreateInfo viewportState = initializers::pipelineViewportStateCreateInfo(1, 1);
	VkPipelineMultisampleStateCreateInfo multisampleState = initializers::pipelineMultisampleStateCreateInfo(VK_SAMPLE_COUNT_1_BIT);
	VkPipelineDynamicStateCreateInfo dynamicState = initializers::pipelineDynamicStateCreateInfo(dynamicStateEnables);

	std::vector<VkVertexInputBindingDescription> vertexBindings = { skybox.vertexBinding(0) };
	std::vector<VkVertexInputAttributeDescription> vertexInputAttributes = skybox.vertexAttributes(0);
	vertexInputAttributes.erase(vertexInputAttributes.begin() + 2);

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

	vkw::ShaderModule vertexShader(shaderPath() + "skybox.vert.spv", VK_SHADER_STAGE_VERTEX_BIT);
	vkw::ShaderModule fragShader(shaderPath() + "skybox.frag.spv", VK_SHADER_STAGE_FRAGMENT_BIT);
	pipelineCreateInfo.shaderStages = { vertexShader.shaderStageInfo(), fragShader.shaderStageInfo() };

	skyboxPipeline.createPipeline(pipelineCreateInfo);

	// reflect
	vertexShader.createShaderModule(shaderPath() + "reflect.vert.spv", VK_SHADER_STAGE_VERTEX_BIT);
	fragShader.createShaderModule(shaderPath() + "reflect.frag.spv", VK_SHADER_STAGE_FRAGMENT_BIT);
	pipelineCreateInfo.shaderStages = { vertexShader.shaderStageInfo(), fragShader.shaderStageInfo() };
	// Enable depth test and write
	depthStencilState.depthWriteEnable = VK_TRUE;
	depthStencilState.depthTestEnable = VK_TRUE;
	// Flip cull mode
	rasterizationState.cullMode = VK_CULL_MODE_FRONT_BIT;

	reflectPipeline.createPipeline(pipelineCreateInfo);
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

				// skybox
				vkCmdBindDescriptorSets(drawCommandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, skyboxDescriptorSet.getPtr(), 0, NULL);
				vkCmdBindVertexBuffers(drawCommandBuffers[i], 0, 1, skybox.vertexBuffer.getPtr(), offsets);
				vkCmdBindIndexBuffer(drawCommandBuffers[i], skybox.indexBuffer, 0, VK_INDEX_TYPE_UINT32);
				vkCmdBindPipeline(drawCommandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, skyboxPipeline);
				vkCmdDrawIndexed(drawCommandBuffers[i], skybox.indexCount, 1, 0, 0, 0);

				// sphere
				vkCmdBindDescriptorSets(drawCommandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, sphereDescriptorSet.getPtr(), 0, NULL);
				vkCmdBindVertexBuffers(drawCommandBuffers[i], 0, 1, sphere.vertexBuffer.getPtr(), offsets);
				vkCmdBindIndexBuffer(drawCommandBuffers[i], sphere.indexBuffer, 0, VK_INDEX_TYPE_UINT32);
				vkCmdBindPipeline(drawCommandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, reflectPipeline);
				vkCmdDrawIndexed(drawCommandBuffers[i], sphere.indexCount, 1, 0, 0, 0);
	
			vkCmdEndRenderPass(drawCommandBuffers[i]);

		drawCommandBuffers[i].endCommandBuffer();
	}
}


void VkwExample::nextFrame() {
	renderFrame();
	
	UBO ubo = {};

	// sphere
	glm::mat4 viewMatrix = glm::mat4(1.0f);
	ubo.projection = glm::perspective(glm::radians(60.0f), (float)swapChain.extent.width / (float)swapChain.extent.width, 0.001f, 256.0f);
	viewMatrix = glm::translate(viewMatrix, glm::vec3(0.0f, 0.0f, -4.0));

	ubo.model = glm::mat4(1.0f);
	ubo.model = viewMatrix * glm::translate(ubo.model, glm::vec3(0, 0, 0));
	ubo.model = glm::rotate(ubo.model, glm::radians(-7.25f), glm::vec3(1.0f, 0.0f, 0.0f));
	ubo.model = glm::rotate(ubo.model, glm::radians(-120.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	ubo.model = glm::rotate(ubo.model, glm::radians(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));

	sphereUniformBuffer.write(&ubo, sizeof(ubo));

	// Skybox
	viewMatrix = glm::mat4(1.0f);
	ubo.projection = glm::perspective(glm::radians(60.0f), (float)swapChain.extent.width / (float)swapChain.extent.width, 0.001f, 256.0f);

	ubo.model = glm::mat4(1.0f);
	ubo.model = viewMatrix * glm::translate(ubo.model, glm::vec3(0, 0, 0));
	ubo.model = glm::rotate(ubo.model, glm::radians(-7.25f), glm::vec3(1.0f, 0.0f, 0.0f));
	ubo.model = glm::rotate(ubo.model, glm::radians(-120.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	ubo.model = glm::rotate(ubo.model, glm::radians(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));

	skyboxUniformBuffer.write(&ubo, sizeof(ubo));
}

struct foo {
	~foo() {
		int i = 10294;
	}
};

int main() {
	foo f;
	GlfwWindow window(1000, 800);

	auto example = VkwExample(window);

	while (!glfwWindowShouldClose(window)) {
		example.nextFrame();
		glfwPollEvents();
	}
}

