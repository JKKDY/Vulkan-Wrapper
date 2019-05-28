#include "ExampleBase.h"


namespace vkx {
	VKAPI_ATTR VkBool32 VKAPI_CALL defaultDebugCallback(
		VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
		VkDebugUtilsMessageTypeFlagsEXT messageType,
		const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
		void* pUserData)
	{
		std::cerr << "validation layer: " << pCallbackData->pMessage << "\n";

		return VK_FALSE;
	}

	std::string getAssetPath() { return "../data/";}
	std::string modelPath() { return getAssetPath() + "models/"; }
	std::string texturePath() { return getAssetPath() + "textures/"; }
	std::string shaderPath() { return "shader/"; }

	void sleep(uint32_t ms)
	{
		std::chrono::milliseconds timespan(ms);
		std::this_thread::sleep_for(timespan);
	}

	bool defaultDeviceSuitable(const vkw::PhysicalDevice & gpu, const vkw::Surface surface)
	{
		bool familiesFound = false;
		for (uint32_t i = 0; i < gpu.queueFamilyProperties.size(); i++) {
			VkBool32 presentSupport = false;
			vkGetPhysicalDeviceSurfaceSupportKHR(gpu, i, surface, &presentSupport);
			if (presentSupport) familiesFound = true;
		}
		familiesFound = gpu.queueFamilyTypes.graphicFamilies.size() * gpu.queueFamilyTypes.transferFamilies.size();

		std::vector<const char*> missingLayers;
		gpu.checkLayers({ VK_KHR_SWAPCHAIN_EXTENSION_NAME }, &missingLayers);

		if (familiesFound * surface.formats(gpu).size() * surface.presentModes(gpu).size() * missingLayers.empty())
			return true;
		else
			return false;
	}

	int defaultRateDevice(const vkw::PhysicalDevice & gpu, const vkw::Surface surface)
	{
		int score = 0;

		if (gpu.properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) score += 1000;
		score += gpu.properties.limits.maxImageDimension2D;  //affects Image Quaity

		for (int index : gpu.queueFamilyTypes.transferFamilies) {
			if (gpu.queueFamilyProperties.at(index).queueFlags == VK_QUEUE_TRANSFER_BIT) {
				score += 1000;
				break;
			}
		}

		for (int index : gpu.queueFamilyTypes.graphicFamilies) {
			VkBool32 presentSupport = false;
			vkGetPhysicalDeviceSurfaceSupportKHR(gpu, index, surface, &presentSupport);
			if (presentSupport)  score += 1000;
		}

		return score;
	}




	ExampleBase::ExampleBase(Window & window):
		window(window)
	{
	}

	ExampleBase::~ExampleBase()
	{
		vkDeviceWaitIdle(device);
	}

	void ExampleBase::initVulkan(const InitInfo & info)
	{
		std::vector<const char*> extensions = info.instanceExtensions;
		window.getWindowExtensions(extensions);
		createInstance(extensions, info.instanceLayers, info.appInfo);
		createSurface();
		choosePhysicalDevice(info.deviceSuitableFkt, info.rateDevicefkt);
		createDevice(info.preSetDeviceQueues, info.additionalDeviceQueues, info.deviceFeatures, info.deviceExtensions);
		createSwapchain();
		createDefaultCmdPools();

		createRenderPrimitives();
		createDepthStencil();
		createRenderPass();
		createPipelineCache();
		createFrameBuffers();
		allocateCommandBuffers();
	}

	void ExampleBase::nextFrame()
	{
		renderFrame();
	}

	void ExampleBase::renderFrame()
	{
		auto startTime = std::chrono::high_resolution_clock::now();

		uint32_t index = swapChain.getNextImage(VK_NULL_HANDLE, renderFence);
		renderFence.wait();
		drawCommandBuffers[index].submitCommandBuffer(device.graphicsQueue, { renderSemaphore });
		swapChain.presentImage(index, { renderSemaphore });

		auto currentTime = std::chrono::high_resolution_clock::now();
		frameTimer = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();
		time += frameTimer;
	}



	void ExampleBase::createInstance(const std::vector<const char*>& extensions, const std::vector<const char*> & layers, VkApplicationInfo * appInfo)
	{
		VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo = vkw::init::debugUtilsMessengerCreateInfoEXT();
		debugCreateInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
		debugCreateInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
		debugCreateInfo.pfnUserCallback = defaultDebugCallback;
		debugCreateInfo.pUserData = nullptr;

		vkw::Instance::CreateInfo instanceCreateInfo = {};
		instanceCreateInfo.debugMessengerInfos = { debugCreateInfo };
		instanceCreateInfo.desiredExtensions = extensions;
		instanceCreateInfo.desiredLayers = layers;
		instanceCreateInfo.desiredExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
		instanceCreateInfo.desiredLayers.push_back("VK_LAYER_LUNARG_standard_validation");

		std::vector<const char*> missingExtensions;
		instanceCreateInfo.desiredExtensions = vkw::Instance::checkExtensions(instanceCreateInfo.desiredExtensions, &missingExtensions);
		VKW_assert(missingExtensions.empty(), "Required Extensions missing");

		std::vector<const char*> missingLayers;
		instanceCreateInfo.desiredLayers = vkw::Instance::checkLayers(instanceCreateInfo.desiredLayers, &missingLayers);
		VKW_assert(missingLayers.empty(), "Required Layers missing");

		instance.createInstance(instanceCreateInfo);
	}

	void ExampleBase::createSurface()
	{
		surface.createSurface(window);
	}

	void ExampleBase::choosePhysicalDevice(std::vector<std::function<bool(const vkw::PhysicalDevice&, const vkw::Surface&)>> deviceSuitableFkt, std::vector<std::function<int(const vkw::PhysicalDevice&, const vkw::Surface&)>> rateDevicefkt)
	{
		VKW_assert(instance.physicalDevices.size(), "failed to find GPUs with Vulkan support!");

		std::map<int, const vkw::PhysicalDevice*> candidates;    
		for (auto & x : instance.physicalDevices) {
			uint32_t suitable = (bool)!deviceSuitableFkt.empty();
			for (auto & fkt : deviceSuitableFkt) suitable *= fkt(x, surface);

			if ((bool)suitable) {
				int score = 0;
				for (auto & fkt : rateDevicefkt) score += fkt(x, surface);
				candidates[score] = &x;		
			}
		}

		VKW_assert(candidates.size(), "failed to suitable GPUs!");

		physicalDevice = *candidates.rbegin()->second;
	}

	void ExampleBase::createDevice(vkw::Device::PreSetQueuesCreateInfo preSetDeviceQueues, std::vector<vkw::Device::AdditionalQueueCreateInfo> additionalDeviceQueues, VkPhysicalDeviceFeatures deviceFeatures, std::vector<const char*> deviceExtensions)
	{
		vkw::Device::CreateInfo deviceCreateInfo = {};
		deviceCreateInfo.physicalDevice = physicalDevice;
		deviceCreateInfo.surfaces = { surface };
		deviceCreateInfo.extensions = deviceExtensions;
		deviceCreateInfo.features = deviceFeatures;
		deviceCreateInfo.preSetQueues = preSetDeviceQueues;
		deviceCreateInfo.additionalQueues = additionalDeviceQueues;

		device.createDevice(deviceCreateInfo);
	}

	void ExampleBase::createSwapchain()
	{
		swapChain.createSwapchain(surface);
	}

	void ExampleBase::createDefaultCmdPools()
	{
		transferCommandPool.createTransferCommandPool();
		graphicsCommandPool.createGraphicsCommandPool();
	}



	void ExampleBase::createRenderPrimitives()
	{
		renderFence.createFence();
		renderSemaphore.createSemaphore();
	}

	void ExampleBase::createRenderPass()
	{
		// Subpass dependencies for layout transitions
		std::vector<VkSubpassDependency> dependencies(2);

		dependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
		dependencies[0].dstSubpass = 0;
		dependencies[0].srcStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
		dependencies[0].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependencies[0].srcAccessMask = VK_ACCESS_MEMORY_READ_BIT;
		dependencies[0].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		dependencies[0].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

		dependencies[1].srcSubpass = 0;
		dependencies[1].dstSubpass = VK_SUBPASS_EXTERNAL;
		dependencies[1].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependencies[1].dstStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
		dependencies[1].srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		dependencies[1].dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
		dependencies[1].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;
	
		std::vector<VkAttachmentDescription> attachments (2);
		// Color attachment
		attachments[0].format = swapChain.surfaceFormat.format;
		attachments[0].samples = VK_SAMPLE_COUNT_1_BIT;
		attachments[0].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		attachments[0].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		attachments[0].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		attachments[0].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		attachments[0].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		attachments[0].finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
		// Depth attachment
		attachments[1].format = vkw::tools::getDepthFormat(physicalDevice);
		attachments[1].samples = VK_SAMPLE_COUNT_1_BIT;
		attachments[1].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		attachments[1].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		attachments[1].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		attachments[1].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		attachments[1].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		attachments[1].finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
	
		VkAttachmentReference colorAttachmentRef = { 0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL };
		VkAttachmentReference depthStencilRef = { 1, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL };
	
		VkSubpassDescription subpass = {};
		subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpass.colorAttachmentCount = 1;
		subpass.pColorAttachments = &colorAttachmentRef;
		subpass.pDepthStencilAttachment = &depthStencilRef;
	
		vkw::RenderPass::CreateInfo renderPassCreateInfo = {};
		renderPassCreateInfo.subPasses.push_back(subpass);
		renderPassCreateInfo.dependencys = dependencies;
		renderPassCreateInfo.attachements = attachments;
	
		renderPass.createRenderPass(renderPassCreateInfo);
	}

	void ExampleBase::createDepthStencil()
	{
		vkw::Image::CreateInfo imageCreateInfo;
		imageCreateInfo.format = vkw::tools::getDepthFormat(physicalDevice);
		imageCreateInfo.extent = { swapChain.extent.width, swapChain.extent.height, 1 };
		imageCreateInfo.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
		imageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
		depthImage.createImage(imageCreateInfo);
		
		depthStencilMemory.allocateMemory(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, {}, {depthImage});

		//depthImage.transitionImageLayout(VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);

		vkw::ImageView::CreateInfo imageViewCreateInfo = {};
		imageViewCreateInfo.image = depthImage;
		imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		imageViewCreateInfo.subresourceRange = {};
		imageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
		imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
		imageViewCreateInfo.subresourceRange.levelCount = 1;
		imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
		imageViewCreateInfo.subresourceRange.layerCount = 1;
		depthImageView.createImageView(imageViewCreateInfo);
	}

	void ExampleBase::createPipelineCache()
	{
		pipelineCache.createPipelineCache();
	}

	void ExampleBase::createFrameBuffers()
	{
		renderFrameBuffers.resize(swapChain.imageCount);


		vkw::FrameBuffer::CreateInfo createInfo = {};
		createInfo.extent = swapChain.extent;
		createInfo.renderPass = renderPass;

		for (uint32_t i = 0; i < renderFrameBuffers.size(); i++) {
			createInfo.attachments = { swapChain.imageView(i), depthImageView };
			renderFrameBuffers[i].createFrameBuffer(createInfo);
		}
	}

	void ExampleBase::allocateCommandBuffers()
	{
		drawCommandBuffers.resize(swapChain.imageCount);
		vkw::CommandBuffer::allocateCommandBuffers(drawCommandBuffers, graphicsCommandPool);
	}
}