#include "vkw_Assets.h"


namespace vkw {

	/// Swapchain
	Swapchain::Swapchain() :
		surfaceFormat(surfaceFormat_m),
		presentMode(presentMode_m),
		extent(extent_m),
		imageCount(imageCount_m)
	{}

	Swapchain::Swapchain(Surface & surface) : Swapchain()
	{
		createSwapchain(surface);
	}

	Swapchain::Swapchain(const CreateInfo & createInfo) : Swapchain() {

	}

	Swapchain::~Swapchain()
	{
		for (auto x : swapChainImageViews) {
			vkDestroyImageView(registry.device, x, nullptr);
		}
	};

	void Swapchain::createSwapchain(Surface & surface) {
		surfaceFormat_m = chooseSwapchainSurfaceFormat(surface.formats(registry.physicalDevice));
		presentMode_m = chooseSwapchainPresentMode(surface.presentModes(registry.physicalDevice));
		extent_m = surface.extent(registry.physicalDevice); //chooseSwapExtent(surface.capabilities, surface.window);			// choose size of swapchain images

		VkSurfaceCapabilitiesKHR capabilities = surface.capabilities(registry.physicalDevice);

		imageCount_m = capabilities.minImageCount + 1;		// choose how many images should be used in the swapchain
		if (capabilities.maxImageCount > 0 && imageCount > capabilities.maxImageCount) {
			imageCount_m = capabilities.maxImageCount;
		}

		VkSwapchainCreateInfoKHR createInfo = init::swapchainCreateInfoKHR();
		createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		createInfo.surface = surface;
		createInfo.minImageCount = imageCount_m;
		createInfo.imageFormat = surfaceFormat.format;
		createInfo.imageColorSpace = surfaceFormat.colorSpace;
		createInfo.imageExtent = extent_m;
		createInfo.imageArrayLayers = 1;
		createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
		createInfo.preTransform = capabilities.currentTransform;
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

		vkw::Debug::errorCodeCheck(vkCreateSwapchainKHR(registry.device, &createInfo, nullptr, pVkObject), "Failed to create Swapchain");

		vkGetSwapchainImagesKHR(registry.device, *pVkObject, &imageCount_m, nullptr); // get the swapchainImages
		swapChainImages.resize(imageCount);
		vkGetSwapchainImagesKHR(registry.device, *pVkObject, &imageCount_m, swapChainImages.data());

		swapChainImageViews.resize(swapChainImages.size()); // create vector with image views for each of the swapchain Images

		for (uint32_t i = 0; i < swapChainImages.size(); i++) { // create the Image views
			VkImageViewCreateInfo imageInfo = init::imageViewCreateInfo();
			imageInfo.components = init::componentMappingSwizzleIdentity();
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

	Swapchain & Swapchain::operator=(const Swapchain & rhs)
	{
		swapChainImages = rhs.swapChainImages;
		swapChainImageViews = rhs.swapChainImageViews;

		surfaceFormat_m = rhs.surfaceFormat_m;
		presentMode_m = rhs.presentMode_m;
		extent_m = rhs.extent_m;
		imageCount_m = rhs.imageCount_m;

		return *this;
	}

	uint32_t Swapchain::getNextImage(VkSemaphore semaphore, VkFence fence)
	{
		uint32_t imageIndex = 0;
		vkAcquireNextImageKHR(registry.device, *pVkObject, std::numeric_limits<uint64_t>::max(), semaphore, fence, &imageIndex);
		return imageIndex;
	}

	void Swapchain::presentImage(uint32_t imageIndex, std::vector<VkSemaphore> semaphores)
	{
		VkPresentInfoKHR presentInfo = init::presentInfoKHR();
		presentInfo.waitSemaphoreCount = static_cast<uint32_t>(semaphores.size());
		presentInfo.pWaitSemaphores = semaphores.data();
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = pVkObject;
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
	Semaphore::Semaphore(const CreateInfo & createInfo):
		Semaphore(createInfo.flags)
	{
	}

	Semaphore::Semaphore(VkSemaphoreCreateFlags flags)
	{
		createSemaphore(flags);
	}

	void Semaphore::createSemaphore(const CreateInfo & createInfo)
	{
		this->flags = createInfo.flags;

		VkSemaphoreCreateInfo info = init::semaphoreCreateInfo();
		info.flags = createInfo.flags;
		info.pNext = createInfo.pNext;
		Debug::errorCodeCheck(vkCreateSemaphore(registry.device, &info, nullptr, pVkObject), "Failed to create Semaphore");
	}

	void Semaphore::createSemaphore(VkSemaphoreCreateFlags flags) {
		this->flags = flags;

		VkSemaphoreCreateInfo createInfo = init::semaphoreCreateInfo();
		createInfo.flags = flags;
		Debug::errorCodeCheck(vkCreateSemaphore(registry.device, &createInfo, nullptr, pVkObject), "Failed to create Semaphore");
	}




	/// Fence
	Fence::Fence(const CreateInfo & createInfo):
		Fence(createInfo.flags)
	{
	}

	Fence::Fence(VkFenceCreateFlags flags)
	{
		createFence(flags);
	}

	void Fence::createFence(const CreateInfo & createInfo)
	{
		this->flags = createInfo.flags;

		VkFenceCreateInfo info = init::fenceCreateInfo();
		info.flags = flags;
		info.pNext = createInfo.pNext;
		Debug::errorCodeCheck(vkCreateFence(registry.device, &info, nullptr, pVkObject), "Failed to create Fence");
	}

	void Fence::createFence(VkFenceCreateFlags flags)
	{
		this->flags = flags;

		VkFenceCreateInfo createInfo = init::fenceCreateInfo();
		createInfo.flags = flags;
		Debug::errorCodeCheck(vkCreateFence(registry.device, &createInfo, nullptr, pVkObject), "Failed to create Fence");
	}

	void Fence::wait(bool reset, uint64_t timeOut)
	{
		Debug::errorCodeCheck(vkWaitForFences(registry.device, 1, pVkObject, VK_TRUE, timeOut), "Failed to wait for Fence");
		if (reset) this->reset();
	}

	void Fence::reset()
	{
		vkResetFences(registry.device, 1, pVkObject);
	}

	void Fence::reset(std::vector<Fence> & fences)
	{
		std::vector<VkFence> vkFences (fences.size());

		for (auto x : fences) vkFences.push_back(x);

		// TODO
		// debug: check if all fences have same registry

		vkResetFences(fences[0].registry.device, static_cast<uint32_t>(vkFences.size()), vkFences.data());
	}





	/// Render Passs
	RenderPass::RenderPass(const CreateInfo & createInfo)
	{
		createRenderPass(createInfo);
	}

	void RenderPass::createRenderPass(const CreateInfo & createInfo)
	{
		subPassDependencys = createInfo.dependencys;
		subPassDescriptions = createInfo.subPasses;
		attachementsDescriptions = createInfo.attachements;
		flags = createInfo.flags;

		VkRenderPassCreateInfo renderPassInfo = init::renderPassCreateInfo();
		renderPassInfo.flags = flags;
		renderPassInfo.pNext = createInfo.pNext;
		renderPassInfo.attachmentCount = static_cast<uint32_t>(attachementsDescriptions.size());
		renderPassInfo.pAttachments = attachementsDescriptions.data();
		renderPassInfo.dependencyCount = static_cast<uint32_t>(subPassDependencys.size());
		renderPassInfo.pDependencies = subPassDependencys.data();
		renderPassInfo.subpassCount = static_cast<uint32_t>(subPassDescriptions.size());
		renderPassInfo.pSubpasses = subPassDescriptions.data();

		Debug::errorCodeCheck(vkCreateRenderPass(registry.device, &renderPassInfo, nullptr, pVkObject), "Failed to create Render Pass");
	}






	/// Shader Module
	ShaderModule::ShaderModule(const CreateInfo & createInfo) :
		ShaderModule(createInfo.filename, createInfo.stage, createInfo.flags)
	{}

	ShaderModule::ShaderModule(std::string filename, VkShaderStageFlagBits stage, VkShaderModuleCreateFlags flags) :
		filename(filename.c_str()), stage(stage), flags(flags)
	{
		createShaderModule(filename, stage, flags);
	}

	void ShaderModule::createShaderModule(const CreateInfo & createInfo)
	{
		this->filename = createInfo.filename.c_str();
		this->stage = createInfo.stage;
		this->flags = createInfo.flags;

		std::ifstream file(this->filename, std::ios::ate | std::ios::binary); // ate: start the end , binary:read the thing in binary

		VKW_assert(file.is_open(), "failed to open shader file for shaderModule! ");

		size_t fileSize = (size_t)file.tellg();
		std::vector<char> shaderCode(fileSize);

		file.seekg(0);
		file.read(shaderCode.data(), fileSize);
		file.close();

		VkShaderModuleCreateInfo info = init::shaderModuleCreateInfo();
		info.codeSize = shaderCode.size();
		info.pCode = reinterpret_cast<const uint32_t*> (shaderCode.data());
		info.pNext = createInfo.pNext;

		Debug::errorCodeCheck(vkCreateShaderModule(registry.device, &info, nullptr, pVkObject), "Failed to create ShaderModule!");
	}

	void ShaderModule::createShaderModule(std::string filename, VkShaderStageFlagBits stage, VkShaderModuleCreateFlags flags)
	{
		CreateInfo createInfo = {};
		createInfo.filename = filename;
		createInfo.stage = stage;
		createInfo.flags = flags;
		createShaderModule(createInfo);
	}

	VkPipelineShaderStageCreateInfo ShaderModule::shaderStageInfo(const VkSpecializationInfo* specializationInfo, const char * name)
	{
		VkPipelineShaderStageCreateInfo createInfo = init::pipelineShaderStageCreateInfo();
		createInfo.module = *pVkObject;
		createInfo.flags = flags;
		createInfo.stage = stage;
		createInfo.pName = name;
		createInfo.pSpecializationInfo = specializationInfo;
		return createInfo;
	}






	/// Pipeline Layout
	PipelineLayout::PipelineLayout(const CreateInfo & createInfo) :
		PipelineLayout(createInfo.setLayouts, createInfo.pushConstants)
	{}

	PipelineLayout::PipelineLayout(const std::vector<VkDescriptorSetLayout> & setLayouts, const std::vector<VkPushConstantRange> & pushConstants)
	{
		createPipelineLayout(setLayouts, pushConstants);
	}

	void PipelineLayout::createPipelineLayout(const CreateInfo & createInfo)
	{
		descriptorSetLayouts = createInfo.setLayouts;
		pushConstantRanges = createInfo.pushConstants;

		VkPipelineLayoutCreateInfo info = init::pipelineLayoutCreateInfo();
		info.pNext = createInfo.pNext;
		info.setLayoutCount = static_cast<uint32_t>(descriptorSetLayouts.size());
		info.pSetLayouts = descriptorSetLayouts.data();
		info.pushConstantRangeCount = static_cast<uint32_t>(pushConstantRanges.size());
		info.pPushConstantRanges = pushConstantRanges.data();

		Debug::errorCodeCheck(vkCreatePipelineLayout(registry.device, &info, nullptr, pVkObject), "failed to create pipeline layout!");
	}

	void PipelineLayout::createPipelineLayout(const std::vector<VkDescriptorSetLayout>& setLayouts, const std::vector<VkPushConstantRange>& pushConstants)
	{
		CreateInfo createInfo = {};
		createInfo.setLayouts = setLayouts;
		createInfo.pushConstants = pushConstants;
		createPipelineLayout(createInfo);
	}







	/// Pipeline Cache
	PipelineCache::PipelineCache(size_t initialSize, void * initialData, VkPipelineCacheCreateFlags flags)
	{
		createPipelineCache(size, data, flags);
	}

	PipelineCache::PipelineCache(const CreateInfo & createInfo)
	{
		createPipelineCache(createInfo);
	}

	void PipelineCache::createPipelineCache(size_t initialSize, void * initialData, VkPipelineCacheCreateFlags flags)
	{
		CreateInfo createInfo = {};
		createInfo.initialSize = size;
		createInfo.initialData = data;
		createInfo.flags = flags;
		createPipelineCache(createInfo);
	}

	void PipelineCache::createPipelineCache(const CreateInfo & createInfo)
	{
		flags = createInfo.flags;
		size = createInfo.initialSize;
		data = createInfo.initialData;

		VkPipelineCacheCreateInfo info = init::pipelineCacheCreateInfo();
		info.flags = createInfo.flags;
		info.initialDataSize = createInfo.initialSize;
		info.pInitialData = createInfo.initialData;
		Debug::errorCodeCheck(vkCreatePipelineCache(registry.device, &info, nullptr, pVkObject));
	}






	/// GraphicsPipeline 
	GraphicsPipeline::GraphicsPipeline(CreateInfo & createInfo)
	{
		createPipeline(createInfo);
	}

	void GraphicsPipeline::createPipeline(const CreateInfo & createInfo)
	{
		shaderStages = createInfo.shaderStages;
		layout = createInfo.layout;
		renderPass = createInfo.renderPass;
		subPass = createInfo.subPass;
		flags = createInfo.flags;
		cache = createInfo.cache;
		basePipelineHandle = createInfo.basePipelineHandle;
		basePipelineIndex = createInfo.basePipelineIndex;

		VkGraphicsPipelineCreateInfo pipelineInfo = vkw::init::graphicsPipelineCreateInfo();
		pipelineInfo.flags = flags;
		pipelineInfo.pNext = createInfo.pNext;
		pipelineInfo.stageCount = static_cast<uint32_t>(shaderStages.size());
		pipelineInfo.pStages = shaderStages.data();
		pipelineInfo.pVertexInputState = createInfo.vertexInputState;
		pipelineInfo.pInputAssemblyState = createInfo.inputAssemblyState;
		pipelineInfo.pTessellationState = createInfo.tessellationState;
		pipelineInfo.pViewportState = createInfo.viewportState;
		pipelineInfo.pRasterizationState = createInfo.rasterizationState;
		pipelineInfo.pMultisampleState = createInfo.multisampleState;
		pipelineInfo.pDepthStencilState = createInfo.depthStencilState;
		pipelineInfo.pColorBlendState = createInfo.colorBlendState;
		pipelineInfo.pDynamicState = createInfo.dynamicState;
		pipelineInfo.layout = layout;
		pipelineInfo.renderPass = renderPass;
		pipelineInfo.subpass = subPass;
		pipelineInfo.basePipelineHandle = basePipelineHandle;
		pipelineInfo.basePipelineIndex = basePipelineIndex;

		Debug::errorCodeCheck(vkCreateGraphicsPipelines(registry.device, cache, 1, &pipelineInfo, nullptr, pVkObject), "Failed to create Pipeline");
	};
}
