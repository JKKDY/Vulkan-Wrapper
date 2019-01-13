#include "vkw_Assets.h"


namespace vkw {

	/// Swapchain
	Swapchain::Swapchain():
		surfaceFormat(surfaceFormat_m),
		presentMode(presentMode_m),
		extent(extent_m),
		imageCount(imageCount_m)
	{}

	Swapchain::Swapchain(Surface & surface) : Swapchain()
	{
		createSwapchain(surface);
	}

	Swapchain::Swapchain(const CreateInfo & createInfo): Swapchain() {
		
	}

	Swapchain::~Swapchain() 
	{
		for (auto x : swapChainImageViews) {
			vkDestroyImageView(registry.device, x, nullptr);
		}
	};

	void Swapchain::createSwapchain(Surface & surface) {
		surfaceFormat_m = chooseSwapchainSurfaceFormat(surface.availableFomats);
		presentMode_m = chooseSwapchainPresentMode(surface.availablePresentModes);
		extent_m = surface.extent; //chooseSwapExtent(surface.capabilities, surface.window);			// choose size of swapchain images

		imageCount_m = surface.capabilities.minImageCount + 1;		// choose how many images should be used in the swapchain
		if (surface.capabilities.maxImageCount > 0 && imageCount > surface.capabilities.maxImageCount) {
			imageCount_m = surface.capabilities.maxImageCount;
		}

		VkSwapchainCreateInfoKHR createInfo = Init::swapchainCreateInfoKHR();
		createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		createInfo.surface = surface;
		createInfo.minImageCount = imageCount_m;
		createInfo.imageFormat = surfaceFormat.format;
		createInfo.imageColorSpace = surfaceFormat.colorSpace;
		createInfo.imageExtent = extent_m;
		createInfo.imageArrayLayers = 1;
		createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
		createInfo.preTransform = surface.capabilities.currentTransform;
		createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		createInfo.presentMode = presentMode_m;
		createInfo.clipped = VK_TRUE;
		createInfo.oldSwapchain = VK_NULL_HANDLE;
		if (registry.graphicsQueue.family != registry.presentQueue.family) { // present and graphics familys are different
			uint32_t queueFamilyIndices[] = { static_cast<uint32_t>(registry.graphicsQueue.family), static_cast<uint32_t>(registry.presentQueue.family) };
			createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
			createInfo.queueFamilyIndexCount = 2;
			createInfo.pQueueFamilyIndices = queueFamilyIndices;
		}
		else { // graphics and present family are the same
			createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
			createInfo.queueFamilyIndexCount = 0; // Optional
			createInfo.pQueueFamilyIndices = nullptr; // Optional
		}

		vkw::Debug::errorCodeCheck(vkCreateSwapchainKHR(registry.device, &createInfo, nullptr, vkObject), "Failed to create Swapchain");

		vkGetSwapchainImagesKHR(registry.device, *vkObject, &imageCount_m, nullptr); // get the swapchainImages
		swapChainImages.resize(imageCount);
		vkGetSwapchainImagesKHR(registry.device, *vkObject, &imageCount_m, swapChainImages.data());

		swapChainImageViews.resize(swapChainImages.size()); // create vector with image views for each of the swapchain Images

		for (uint32_t i = 0; i < swapChainImages.size(); i++) { // create the Image views
			VkImageViewCreateInfo imageInfo = Init::imageViewCreateInfo();
			imageInfo.components = Init::componentMappingSwizzleIdentity();
			imageInfo.format = surfaceFormat.format;
			imageInfo.image = swapChainImages[i];
			imageInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
			imageInfo.subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };
			imageInfo.flags = 0;
			vkCreateImageView(registry.device, &imageInfo, nullptr, &swapChainImageViews[i]);
		}
	}

	void Swapchain::createSwapchain(const CreateInfo & createInfo) {
		
	}

	uint32_t Swapchain::getNextImage(VkSemaphore semaphore, VkFence fence)
	{
		uint32_t imageIndex = 0;
		vkAcquireNextImageKHR(registry.device, *vkObject, std::numeric_limits<uint64_t>::max(), semaphore, fence, &imageIndex);
		return imageIndex;
	}

	void Swapchain::presentImage(uint32_t imageIndex, std::vector<VkSemaphore> semaphores)
	{
		VkPresentInfoKHR presentInfo = Init::presentInfoKHR();
		presentInfo.waitSemaphoreCount = static_cast<uint32_t>(semaphores.size());
		presentInfo.pWaitSemaphores = semaphores.data();
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = vkObject;
		presentInfo.pImageIndices = &imageIndex;

		vkQueuePresentKHR(registry.presentQueue, &presentInfo);
	}

	VkImageView Swapchain::imageView(uint32_t i)
	{
		return swapChainImageViews[i];
	}

	VkSurfaceFormatKHR Swapchain::chooseSwapchainSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats)
	{
		if (availableFormats.size() == 1 && availableFormats[0].format == VK_FORMAT_UNDEFINED) {	// if no format is defined
			return{ VK_FORMAT_B8G8R8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR };					// take VK_FORMAT_B8G8R8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR
		}

		for (const auto& availableFormat : availableFormats) {
			if (availableFormat.format == VK_FORMAT_B8G8R8A8_UNORM && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) { // VK_FORMAT_B8G8R8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR exists
				return availableFormat;		//take VK_FORMAT_B8G8R8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR
			}
		}

		return availableFormats[0]; // otherwise return the first format
	}

	VkPresentModeKHR Swapchain::chooseSwapchainPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes)
	{
		for (const auto& availablePresentMode : availablePresentModes) {
			if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {		// if VK_PRESENT_MODE_MAILBOX_KHR 
				return availablePresentMode;								// take VK_PRESENT_MODE_MAILBOX_KHR
			}
		}

		return VK_PRESENT_MODE_FIFO_KHR;	// else take VK_PRESENT_MODE_FIFO_KHR (this presentmode is always supported)
	}
												




	/// Semaphore
	Semaphore::Semaphore(VkSemaphoreCreateFlags flags)
	{
		createSemaphore(flags);
	}

	void Semaphore::createSemaphore(VkSemaphoreCreateFlags flags) {
		VkSemaphoreCreateInfo createInfo = Init::semaphoreCreateInfo();
		createInfo.flags = flags;
		Debug::errorCodeCheck(vkCreateSemaphore(registry.device, &createInfo, nullptr, vkObject), "Failed to create Semaphore");
	}





	/// Fence
	Fence::Fence(VkFenceCreateFlags flags)
	{
		VkFenceCreateInfo createInfo = Init::fenceCreateInfo();
		createInfo.flags = flags;
		Debug::errorCodeCheck(vkCreateFence(registry.device, &createInfo, nullptr, vkObject), "Failed to create Fence");
	}

	void Fence::wait(bool reset, uint64_t timeOut)
	{
		Debug::errorCodeCheck(vkWaitForFences(registry.device, 1, vkObject, VK_TRUE, timeOut), "Failed to wait for Fence");
		if (reset) { this->reset(); }
	}

	void Fence::reset()
	{
		vkResetFences(registry.device, 1, vkObject);
	}





	/// Render Passs
	RenderPass::RenderPass(CreateInfo & createInfo):
		subPassDependencys(createInfo.dependencys),
		subPassDescriptions(createInfo.subPasses),
		attachementsDescriptions(createInfo.attachements),
		flags(createInfo.flags)
	{
		createRenderPass();
	}

	void RenderPass::createRenderPass()
	{
		
		VkRenderPassCreateInfo renderPassInfo = Init::renderPassCreateInfo();
		renderPassInfo.flags = flags;
		renderPassInfo.attachmentCount = static_cast<uint32_t>(attachementsDescriptions.size());
		renderPassInfo.pAttachments = attachementsDescriptions.data();
		renderPassInfo.dependencyCount = static_cast<uint32_t>(subPassDependencys.size());
		renderPassInfo.pDependencies = subPassDependencys.data();
		renderPassInfo.subpassCount = static_cast<uint32_t>(subPassDescriptions.size());
		renderPassInfo.pSubpasses = subPassDescriptions.data();

		Debug::errorCodeCheck(vkCreateRenderPass(registry.device, &renderPassInfo, nullptr, vkObject), "Failed to create Render Pass");
	}





	/// Shader Module
	ShaderModule::ShaderModule(std::string filename, VkShaderStageFlagBits stage, VkShaderModuleCreateFlags flags) :
		filename(filename.c_str()), stage(stage), flags(flags)
	{
		createShaderModule();
	}

	void ShaderModule::createShaderModule()
	{
		
		std::ifstream file(filename, std::ios::ate | std::ios::binary); // ate: start the end , binary:read the thing in binary

		VKW_assert(file.is_open(), "failed to open shader file for shaderModule! ");

		size_t fileSize = (size_t)file.tellg();
		std::vector<char> shaderCode(fileSize);

		file.seekg(0);
		file.read(shaderCode.data(), fileSize);
		file.close();

		VkShaderModuleCreateInfo createInfo = Init::shaderModuleCreateInfo();
		createInfo.codeSize = shaderCode.size();
		createInfo.pCode = reinterpret_cast<const uint32_t*> (shaderCode.data());

		Debug::errorCodeCheck(vkCreateShaderModule(registry.device, &createInfo, nullptr, vkObject), "Failed to create ShaderModule!");
	}

	VkPipelineShaderStageCreateInfo ShaderModule::pipelineShaderStageCreateInfo(const VkSpecializationInfo* specializationInfo)
	{
		VkPipelineShaderStageCreateInfo createInfo = Init::pipelineShaderStageCreateInfo();
		createInfo.module = *vkObject;
		createInfo.flags = flags;
		createInfo.stage = stage;
		createInfo.pName = "main";
		createInfo.pSpecializationInfo = specializationInfo;
		return createInfo;
	}






	/// Pipeline Layout
	PipelineLayout::PipelineLayout(std::vector<VkDescriptorSetLayout> setLayouts, std::vector<VkPushConstantRange> pushConstants)
	{
		descriptorSetLayouts = setLayouts;
		pushConstantRanges = pushConstants;
		createPipelineLayout();
	}

	void PipelineLayout::createPipelineLayout()
	{
		VkPipelineLayoutCreateInfo createInfo = Init::pipelineLayoutCreateInfo();
		createInfo.setLayoutCount = static_cast<uint32_t>(descriptorSetLayouts.size());
		createInfo.pSetLayouts = descriptorSetLayouts.data();
		createInfo.pushConstantRangeCount = static_cast<uint32_t>(pushConstantRanges.size());
		createInfo.pPushConstantRanges = pushConstantRanges.data();

		Debug::errorCodeCheck(vkCreatePipelineLayout(registry.device, &createInfo, nullptr, vkObject), "failed to create pipeline layout!");
	}






	/// Pipeline Cache
	PipelineCache::PipelineCache(size_t size, void * data, VkPipelineCacheCreateFlags flags) :
		size(size), data(data), flags(flags)
	{
		createPipelineCache();
	}

	void PipelineCache::createPipelineCache()
	{
		VkPipelineCacheCreateInfo createInfo = Init::pipelineCacheCreateInfo();
		createInfo.flags = flags;
		createInfo.initialDataSize = size;
		createInfo.pInitialData = data;
		Debug::errorCodeCheck(vkCreatePipelineCache(registry.device, &createInfo, nullptr, vkObject));
	}






	/// GraphicsPipeline 
	GraphicsPipeline::GraphicsPipeline(CreateInfo & createInfo) :
		shaderStages(createInfo.shaderStages),
		layout(createInfo.layout),
		renderPass(createInfo.renderPass),
		subPass(createInfo.subPass),
		flags(createInfo.flags),
		cache(createInfo.cache),
		basePipelineIndex(createInfo.basePipelineIndex),
		basePipelineHandle(createInfo.basePipelineHandle),
		pipelineStates(createInfo.pipelineStates)
	{
		createPipeline();
	}

	void GraphicsPipeline::createPipeline()
	{
		

		VkGraphicsPipelineCreateInfo pipelineInfo = {};
		pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;

		pipelineInfo.flags = flags;
		pipelineInfo.stageCount = static_cast<uint32_t>(shaderStages.size());
		pipelineInfo.pStages = shaderStages.data();

		if (pipelineStates.vertexInputState.sType == VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO) {
			pipelineInfo.pVertexInputState = &pipelineStates.vertexInputState;
		}
		if (pipelineStates.inputAssemblyState.sType == VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO) {
			pipelineInfo.pInputAssemblyState = &pipelineStates.inputAssemblyState;
		}
		if (pipelineStates.tessellationState.sType == VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO) {
			pipelineInfo.pTessellationState = &pipelineStates.tessellationState;
		}
		if (pipelineStates.viewportState.sType == VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO) {
			pipelineInfo.pViewportState = &pipelineStates.viewportState;
		}
		if (pipelineStates.rasterizationState.sType == VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO) {
			pipelineInfo.pRasterizationState = &pipelineStates.rasterizationState;
		}
		if (pipelineStates.multisampleState.sType == VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO) {
			pipelineInfo.pMultisampleState = &pipelineStates.multisampleState;
		}
		if (pipelineStates.depthStencilState.sType == VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO) {
			pipelineInfo.pDepthStencilState = &pipelineStates.depthStencilState;
		}
		if (pipelineStates.colorBlendState.sType == VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO) {
			pipelineInfo.pColorBlendState = &pipelineStates.colorBlendState;
		}
		if (pipelineStates.dynamicState.sType == VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO) {
			pipelineInfo.pDynamicState = &pipelineStates.dynamicState;
		}

		pipelineInfo.layout = layout;
		pipelineInfo.renderPass = renderPass;
		pipelineInfo.subpass = subPass;
		pipelineInfo.basePipelineHandle = basePipelineHandle;
		pipelineInfo.basePipelineIndex = basePipelineIndex;

		Debug::errorCodeCheck(vkCreateGraphicsPipelines(registry.device, cache, 1, &pipelineInfo, nullptr, vkObject), "Failed to create Pipeline");
	};
}
