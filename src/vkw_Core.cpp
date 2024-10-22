#include "vkw_Core.h"

#define INVALID_PRIORITY -1.0f

namespace vkw {

	/// Window
	Window::~Window() { }


	Window::operator void* () { return window; }



	/// PhysicalDevice
	PhysicalDevice::PhysicalDevice(VkPhysicalDevice gpu, 
		const std::vector<VkQueueFamilyProperties> & queueFamilies, 
		const VkPhysicalDeviceProperties & props, 
		const VkPhysicalDeviceFeatures & featr, 
		const VkPhysicalDeviceMemoryProperties & memProp,
		const QueueFamilyTypes & famTypes) :
		physicalDevice(gpu),
		queueFamilyProperties(queueFamilies),
		properties(props),
		features(featr),
		memoryProperties(memProp),
		queueFamilyTypes(famTypes)
	{}

	PhysicalDevice::operator VkPhysicalDevice () const
	{
		return physicalDevice;
	}

	std::vector<const char*> PhysicalDevice::checkLayers(const std::vector<const char*>& desiredLayers, std::vector<const char*>* outMissingLayers) const
	{
		auto func = [=](uint32_t * count, VkExtensionProperties * prop) { return vkEnumerateDeviceExtensionProperties( physicalDevice, nullptr, count, prop); };
		return tools::check<VkExtensionProperties>(func, [](const VkExtensionProperties & t) {return t.extensionName; }, desiredLayers, outMissingLayers);
	}

	VkPhysicalDeviceFeatures PhysicalDevice::checkFeatures(const VkPhysicalDeviceFeatures & desiredFeatures, VkPhysicalDeviceFeatures * outMissingFeatures)
	{
		VkPhysicalDeviceFeatures existngFeatures = {};

#define CHECK_FEATURE(name)existngFeatures.##name = (desiredFeatures.##name && features.##name); \
							if (outMissingFeatures != nullptr) outMissingFeatures->##name = (desiredFeatures.##name && !features.##name);

		CHECK_FEATURE(robustBufferAccess);
		CHECK_FEATURE(fullDrawIndexUint32);
		CHECK_FEATURE(imageCubeArray);
		CHECK_FEATURE(independentBlend);
		CHECK_FEATURE(geometryShader);
		CHECK_FEATURE(tessellationShader);
		CHECK_FEATURE(sampleRateShading);
		CHECK_FEATURE(dualSrcBlend);
		CHECK_FEATURE(logicOp);
		CHECK_FEATURE(multiDrawIndirect);
		CHECK_FEATURE(drawIndirectFirstInstance);
		CHECK_FEATURE(depthClamp);
		CHECK_FEATURE(depthBiasClamp);
		CHECK_FEATURE(fillModeNonSolid);
		CHECK_FEATURE(depthBounds);
		CHECK_FEATURE(wideLines);
		CHECK_FEATURE(largePoints);
		CHECK_FEATURE(alphaToOne);
		CHECK_FEATURE(multiViewport);
		CHECK_FEATURE(samplerAnisotropy);
		CHECK_FEATURE(textureCompressionETC2);
		CHECK_FEATURE(textureCompressionASTC_LDR);
		CHECK_FEATURE(textureCompressionBC);
		CHECK_FEATURE(occlusionQueryPrecise);
		CHECK_FEATURE(pipelineStatisticsQuery);
		CHECK_FEATURE(vertexPipelineStoresAndAtomics);
		CHECK_FEATURE(fragmentStoresAndAtomics);
		CHECK_FEATURE(shaderTessellationAndGeometryPointSize);
		CHECK_FEATURE(shaderImageGatherExtended);
		CHECK_FEATURE(shaderStorageImageExtendedFormats);
		CHECK_FEATURE(shaderStorageImageMultisample);
		CHECK_FEATURE(shaderStorageImageReadWithoutFormat);
		CHECK_FEATURE(shaderStorageImageWriteWithoutFormat);
		CHECK_FEATURE(shaderUniformBufferArrayDynamicIndexing);
		CHECK_FEATURE(shaderSampledImageArrayDynamicIndexing);
		CHECK_FEATURE(shaderStorageBufferArrayDynamicIndexing);
		CHECK_FEATURE(shaderStorageImageArrayDynamicIndexing);
		CHECK_FEATURE(shaderClipDistance);
		CHECK_FEATURE(shaderCullDistance);
		CHECK_FEATURE(shaderFloat64);
		CHECK_FEATURE(shaderInt64);
		CHECK_FEATURE(shaderInt16);
		CHECK_FEATURE(shaderResourceResidency);
		CHECK_FEATURE(shaderResourceMinLod);
		CHECK_FEATURE(sparseBinding);
		CHECK_FEATURE(sparseResidencyBuffer);
		CHECK_FEATURE(sparseResidencyImage2D);
		CHECK_FEATURE(sparseResidencyImage3D);
		CHECK_FEATURE(sparseResidency2Samples);
		CHECK_FEATURE(sparseResidency4Samples);
		CHECK_FEATURE(sparseResidency8Samples);
		CHECK_FEATURE(sparseResidency16Samples);
		CHECK_FEATURE(sparseResidencyAliased);
		CHECK_FEATURE(variableMultisampleRate);
		CHECK_FEATURE(inheritedQueries);

#undef CHECK_FEATURE

		return existngFeatures;
	}
	




	/// Instance
	Instance::Instance() : 
		physicalDevices(physicalDevices_m)
	{}

	Instance::Instance(const CreateInfo & createInfo):Instance()
	{
		createInstance(createInfo);
	}

	Instance::~Instance()
	{
		for (auto x : debugMessengers) destroyDebugUtilsMessengerEXT(x, nullptr);
	}

	void Instance::createInstance(const CreateInfo & createInfo)
	{
		VkInstanceCreateInfo instanceCreateInfo = init::instanceCreateInfo();   // instance create info: app info, extensions and layers required
		instanceCreateInfo.pApplicationInfo = &createInfo.appInfo;
		instanceCreateInfo.enabledLayerCount = static_cast<uint32_t>(createInfo.desiredLayers.size());
		instanceCreateInfo.ppEnabledLayerNames = createInfo.desiredLayers.data();
		instanceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(createInfo.desiredExtensions.size());
		instanceCreateInfo.ppEnabledExtensionNames = createInfo.desiredExtensions.data();
		instanceCreateInfo.pNext = createInfo.pNext;

		Debug::errorCodeCheck(vkCreateInstance(&instanceCreateInfo, nullptr, pVkObject.createNew()), "Failed to create Instance");

		debugMessengers.resize(createInfo.debugMessengerInfos.size());
		for (size_t i = 0; i < debugMessengers.size(); i++) {
			Debug::errorCodeCheck(createDebugUtilsMessengerEXT(createInfo.debugMessengerInfos.at(i), nullptr, debugMessengers.at(i), false), std::string("Failed to create Debug Messenger, index: ",  i).c_str());
		}

		vkw::DebugInformationPrint::printSystemInformation(this->registry.instance);

		uint32_t deviceCount;
		vkEnumeratePhysicalDevices(registry.instance, &deviceCount, nullptr);
		assert(deviceCount);
		std::vector<VkPhysicalDevice> physDevices(deviceCount);
		physicalDevices_m.reserve(deviceCount);
		vkEnumeratePhysicalDevices(registry.instance, &deviceCount, physDevices.data());

		for (auto & x : physDevices) {
			VkPhysicalDeviceProperties prop = {};
			VkPhysicalDeviceFeatures features = {};
			VkPhysicalDeviceMemoryProperties memProp = {};
			std::vector<VkQueueFamilyProperties> queueFamilyProperties;
			QueueFamilyTypes queueFamTypes;

			uint32_t queueFamCount;
			vkGetPhysicalDeviceQueueFamilyProperties(x, &queueFamCount, nullptr);
			queueFamilyProperties.resize(queueFamCount);
			vkGetPhysicalDeviceQueueFamilyProperties(x, &queueFamCount, queueFamilyProperties.data());

			vkGetPhysicalDeviceProperties(x, &prop);
			vkGetPhysicalDeviceFeatures(x, &features);
			vkGetPhysicalDeviceMemoryProperties(x, &memProp);

			for (uint32_t i = 0; i < queueFamCount; i++ ) {  // do for all queue families
				if (queueFamilyProperties.at(i).queueCount > 0) {
					if (queueFamilyProperties.at(i).queueFlags & VK_QUEUE_GRAPHICS_BIT) queueFamTypes.graphicFamilies.push_back(i);
					if (queueFamilyProperties.at(i).queueFlags & VK_QUEUE_COMPUTE_BIT) queueFamTypes.computeFamilies.push_back(i);
					if (queueFamilyProperties.at(i).queueFlags & VK_QUEUE_TRANSFER_BIT) queueFamTypes.transferFamilies.push_back(i);
					if (queueFamilyProperties.at(i).queueFlags & VK_QUEUE_SPARSE_BINDING_BIT) queueFamTypes.sparseBindingFamilies.push_back(i);
					if (queueFamilyProperties.at(i).queueFlags & VK_QUEUE_PROTECTED_BIT) queueFamTypes.protectedFamilies.push_back(i);
				}
			}

			physicalDevices_m.emplace_back(x, queueFamilyProperties, prop, features, memProp, queueFamTypes);
		}
	}

	impl::RegistryManager & Instance::getRegistry()
	{
		return this->registry;
	}

	VkResult Instance::createDebugUtilsMessengerEXT(const VkDebugUtilsMessengerCreateInfoEXT & pCreateInfo, const VkAllocationCallbacks * pAllocator, VkDebugUtilsMessengerEXT & pCallback, bool automaticDestruction) {
		auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(*pVkObject, "vkCreateDebugUtilsMessengerEXT");
		if (func != nullptr) {
			auto ret = func(*pVkObject, &pCreateInfo, pAllocator, &pCallback);
			if (automaticDestruction && ret == VK_SUCCESS) debugMessengers.push_back(pCallback);
			return ret;
		}
		else {
			VKW_PRINT("failed to fetch vkCreateDebugUtilsMessengerEXT function pointer");
			return VK_ERROR_EXTENSION_NOT_PRESENT;
		}
	}

	void Instance::destroyDebugUtilsMessengerEXT(VkDebugUtilsMessengerEXT callback, const VkAllocationCallbacks* pAllocator) {
		auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(*pVkObject, "vkDestroyDebugUtilsMessengerEXT");
		if (func != nullptr) {
			auto it = std::find(debugMessengers.begin(), debugMessengers.end(), callback);
			if (it != debugMessengers.end()) debugMessengers.erase(it);
			func(*pVkObject, callback, pAllocator);
		}
		else {
			VKW_PRINT("failed to destory debugUtilsMessengerEXT");
		}
	}

	std::vector<const char*> Instance::checkExtensions(const std::vector<const char*> & desiredExtensions, std::vector<const char*> * outMissingExtensions)
	{
		auto func = [](uint32_t * count, VkExtensionProperties * prop) { return vkEnumerateInstanceExtensionProperties(nullptr, count, prop); };
		return tools::check<VkExtensionProperties>(func, [](const VkExtensionProperties & t) {return t.extensionName; }, desiredExtensions, outMissingExtensions);
	}

	std::vector<const char*> Instance::checkLayers(const std::vector<const char*> & desiredLayers, std::vector<const char*>* outMissingLayers)
	{
		auto func = [](uint32_t * count, VkLayerProperties * prop) { return vkEnumerateInstanceLayerProperties(count, prop); };
		return tools::check<VkLayerProperties>(func, [](const VkLayerProperties & t) {return t.layerName; }, desiredLayers, outMissingLayers);
	}




	

	/// Surface
	Surface::Surface(const CreateInfo & createInfo)
	{
		createSurface(createInfo);
	}

	Surface::Surface(const vkw::Window & window) 
	{
		createSurface(window);
	}

	void Surface::createSurface(const CreateInfo & createInfo)
	{
		createSurface(createInfo.window);
	}

	void Surface::createSurface(const Window & window)
	{
		this->window = &window;
		window.createSurface(registry.instance, pVkObject.createNew());
	}

	std::vector<VkSurfaceFormatKHR> Surface::formats(VkPhysicalDevice gpu)const
	{	
		if (queriedFormats.count(gpu) != 0) return queriedFormats.at(gpu);
		else {
			std::vector<VkSurfaceFormatKHR> availableFormats;

			uint32_t formatCount = 0;
			vkGetPhysicalDeviceSurfaceFormatsKHR(gpu, *pVkObject, &formatCount, nullptr);
			availableFormats.resize(formatCount);
			vkGetPhysicalDeviceSurfaceFormatsKHR(gpu, *pVkObject, &formatCount, availableFormats.data());

			queriedFormats[gpu] = availableFormats;

			return availableFormats;
		}
	}

	std::vector<VkPresentModeKHR> Surface::presentModes(VkPhysicalDevice gpu)const
	{
		if (queriedPresentModes.count(gpu) != 0) return queriedPresentModes.at(gpu);
		else {
			std::vector<VkPresentModeKHR> availablePresentModes;

			uint32_t presentModeCount = 0;
			vkGetPhysicalDeviceSurfacePresentModesKHR(gpu, *pVkObject, &presentModeCount, nullptr);
			availablePresentModes.resize(presentModeCount);
			vkGetPhysicalDeviceSurfacePresentModesKHR(gpu, *pVkObject, &presentModeCount, availablePresentModes.data());

			queriedPresentModes[gpu] = availablePresentModes;

			return availablePresentModes;
		}
	}

	VkSurfaceCapabilitiesKHR Surface::capabilities(VkPhysicalDevice gpu) const
	{
		if (queriedCapabilities.count(gpu) != 0) return queriedCapabilities.at(gpu);
		else {

			VkSurfaceCapabilitiesKHR availableCapabilities;
			vkGetPhysicalDeviceSurfaceCapabilitiesKHR(gpu, *pVkObject, &availableCapabilities);

			queriedCapabilities[gpu] = availableCapabilities;

			return availableCapabilities;
		}
	}

	VkExtent2D Surface::extent(VkPhysicalDevice gpu) const
	{
		VkExtent2D currentExtent;
		VkSurfaceCapabilitiesKHR cap;
		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(gpu, *pVkObject, &cap);

		if (cap.currentExtent.width != std::numeric_limits<uint32_t>::max()) currentExtent = cap.currentExtent;
		else {
			int height, width;

			window->getWindowSize(&height, &width);

			currentExtent = { static_cast<uint32_t>(width),  static_cast<uint32_t>(height) };

			currentExtent.width = std::max(cap.minImageExtent.width, std::min(cap.maxImageExtent.width, currentExtent.width));
			currentExtent.height = std::max(cap.minImageExtent.height, std::min(cap.maxImageExtent.height, currentExtent.height));
		}

		return currentExtent;
	}






	/// LogicalDevice
	Device::QueueInfo::operator VkQueue() const
	{
		return queue;
	}

	Device::Device(): 
		graphicsQueue(graphicsQueue_m),
		transferQueue(transferQueue_m),
		presentQueue(presentQueue_m),
		computeQueue(computeQueue_m),
		additionalQueues(additionalQueues_m),
		deviceRegistry(registry.createNewRegistry())
	{}

	Device::Device(const CreateInfo & createInfo) : Device()
	{		
		createDevice(createInfo);
	}

	void Device::createDevice(const CreateInfo & createInfo)
	{	
		std::map<int, std::vector<float>> priorities;
		std::vector<VkDeviceQueueCreateInfo> queueCreateInfos = setupQueueCreation(createInfo, priorities);

		VkDeviceCreateInfo deviceInfo = init::deviceCreateInfo(); // create info with the queueCreateInfos, enabled device featurs and extenions
		deviceInfo.pQueueCreateInfos = queueCreateInfos.data();
		deviceInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
		deviceInfo.pEnabledFeatures = &createInfo.features;
		deviceInfo.enabledExtensionCount = static_cast<uint32_t>(createInfo.extensions.size());
		deviceInfo.ppEnabledExtensionNames = createInfo.extensions.data();
		deviceInfo.enabledLayerCount = 0;
		deviceInfo.ppEnabledLayerNames = nullptr;
		deviceInfo.pNext = createInfo.pNext;

		Debug::errorCodeCheck(vkCreateDevice(createInfo.physicalDevice.physicalDevice, &deviceInfo, nullptr, pVkObject.createNew()), "Failed to create Device");

		if (graphicsQueue_m.family >= 0) vkGetDeviceQueue(*pVkObject, graphicsQueue_m.family, graphicsQueue_m.index, &graphicsQueue_m.queue);
		if (transferQueue_m.family >= 0) vkGetDeviceQueue(*pVkObject, transferQueue_m.family, transferQueue_m.index, &transferQueue_m.queue);
		if (presentQueue_m.family >= 0) vkGetDeviceQueue(*pVkObject, presentQueue_m.family, presentQueue_m.index,  &presentQueue_m.queue);
		if (computeQueue_m.family >= 0) vkGetDeviceQueue(*pVkObject, computeQueue_m.family, computeQueue_m.index, &computeQueue_m.queue);

		for (auto & x : createInfo.additionalQueues) {
			for (uint32_t i = 0; i < x.priorities.size(); i++) {
				QueueInfo queueInfo = {};
				queueInfo.family = x.family;
				queueInfo.index = x.index + i;
				queueInfo.priority = priorities[x.family][x.index + i];
				vkGetDeviceQueue(*pVkObject, x.family, queueInfo.index, &queueInfo.queue);
				additionalQueues_m.push_back(queueInfo);
			}
		}

		createRegistry(createInfo);
	}

	impl::Registry & Device::getRegistry()
	{
		return *this->deviceRegistry;
	}

	void Device::createRegistry(const CreateInfo & createInfo)
	{
		impl::PhysicalDevice gpu = {
			createInfo.physicalDevice,
			createInfo.physicalDevice.queueFamilyProperties,
			createInfo.physicalDevice.properties,
			createInfo.physicalDevice.features,
			createInfo.physicalDevice.memoryProperties
		};

		impl::DeviceQueue graphics = {
			graphicsQueue,
			graphicsQueue.family
		};

		impl::DeviceQueue compute = {
			computeQueue,
			computeQueue.family
		};

		impl::DeviceQueue transfer = {
			transferQueue,
			transferQueue.family
		};

		impl::DeviceQueue present = {
			presentQueue,
			presentQueue.family
		};

		deviceRegistry->initialize(*pVkObject, graphics, transfer, present, compute, gpu);
	}

	std::vector<VkDeviceQueueCreateInfo> Device::setupPresetQueues(const PhysicalDevice & gpu, const PreSetQueuesCreateInfo & presetQueues, std::map<int, std::vector<float>> & priorities, const std::vector<VkSurfaceKHR> & surfaces)
	{
		std::vector<VkDeviceQueueCreateInfo> createInfos;

		auto addUserQueueCreateInfo = [&](QueueInfo & info, const PreSetQueueCreateInfo & userQueue) {
			info.family = userQueue.family;
			info.index = userQueue.index == std::numeric_limits<uint32_t>::max() ? 0 : userQueue.index;
			info.priority = userQueue.priority;

			auto it = std::find_if(createInfos.begin(), createInfos.end(), [&](const VkDeviceQueueCreateInfo & createInfo) {return createInfo.queueFamilyIndex == userQueue.family; });
			if (it != createInfos.end()) { // if CreateInfo with userQueue.family found (i.e. duplicate)

				int sizeDif = (info.index + 1) - it->queueCount;

				if (sizeDif > 0) {	// if index is higher than queueCount (i.e more queues need to be created for index to be valid)
					for (int i = 0; i < sizeDif - 1; i++) priorities[userQueue.family].push_back(INVALID_PRIORITY);
					priorities[userQueue.family].push_back(userQueue.priority);
				}
				else {		// index is not higher than queue count (i.e. amount of queues is sufficient)
					priorities[userQueue.family][info.index] = userQueue.priority;
				}

				it->pQueuePriorities = priorities[userQueue.family].data();
				it->queueCount = static_cast<uint32_t>(priorities[userQueue.family].size());
			}
			else { // if CreateInfo with userQueue.family not found (i.e. no duplicate)
				for (int i = 0; i < info.index; i++) {
					priorities[userQueue.family].push_back(INVALID_PRIORITY);
				}
				priorities[userQueue.family].push_back(userQueue.priority);
				VkDeviceQueueCreateInfo queueCreateInfo = init::deviceQueueCreateInfo();
				queueCreateInfo.queueFamilyIndex = info.family;
				queueCreateInfo.queueCount = static_cast<uint32_t>(priorities[userQueue.family].size());
				queueCreateInfo.pQueuePriorities = priorities[userQueue.family].data();
				queueCreateInfo.flags = 0;
				createInfos.push_back(queueCreateInfo);
			}
		};


		auto addQueueCreateInfo = [&](QueueInfo & info, int family) {
			info.family = family;
			info.index = 0;
			info.priority = 1.0f;

			// if CreateInfo with this family not found (i.e. no duplicate)
			auto it = std::find_if(createInfos.begin(), createInfos.end(), [&](const VkDeviceQueueCreateInfo & createInfo) {return createInfo.queueFamilyIndex == family; });
			if (it == createInfos.end()) { 
				// create new createInfo
				priorities[family].push_back(info.priority);
				VkDeviceQueueCreateInfo queueCreateInfo = init::deviceQueueCreateInfo();
				queueCreateInfo.queueFamilyIndex = info.family;
				queueCreateInfo.queueCount = 1;
				queueCreateInfo.pQueuePriorities = priorities[family].data();
				queueCreateInfo.flags = 0;
				createInfos.push_back(queueCreateInfo);
			}
			else { 
				// NOTE: test this, the [0] could cause a bug
				priorities[family][0] = info.priority;
				it->pQueuePriorities = priorities[family].data();
			}
		};


		auto checkPresentSupport = [&](int ind) {
			VkBool32 presentSupport = VK_TRUE * !surfaces.empty();
			for (auto & x : surfaces) {
				VkBool32 p = VK_FALSE;
				vkGetPhysicalDeviceSurfaceSupportKHR(gpu.physicalDevice, ind, x, &p);
				presentSupport *= p;
			}
			return presentSupport;
		};


		auto findPresentFamily = [&]() {
			int index = -1;

			for (uint32_t i = 0; i < gpu.queueFamilyProperties.size(); i++) {
				VkBool32 presentSupport = checkPresentSupport(i);
				if (presentSupport) {
					index = i;
					break;
				}
			}

			return index;
		};



		// compute Queue
		if (presetQueues.createCompute) { // if create computeQueue
			if (presetQueues.compute.family != std::numeric_limits<uint32_t>::max()) // if use user specified Info
				addUserQueueCreateInfo(computeQueue_m, presetQueues.compute);
			else {	
				auto onlyCompute = std::find_if(gpu.queueFamilyProperties.begin(), gpu.queueFamilyProperties.end(), [](const VkQueueFamilyProperties & prop) {return prop.queueFlags == VK_QUEUE_COMPUTE_BIT; }); // look for a compute only queueu

				if (onlyCompute != gpu.queueFamilyProperties.end()) // if compute only queue found
					addQueueCreateInfo(computeQueue_m, static_cast<uint32_t>(std::distance(gpu.queueFamilyProperties.begin(), onlyCompute)));
				else if (!gpu.queueFamilyTypes.computeFamilies.empty())  // if a compute queue exists
					addQueueCreateInfo(computeQueue_m, gpu.queueFamilyTypes.computeFamilies[0]); // otheriwse no queue will be created
			}
		}

		// Transfer Queue
		if (presetQueues.createTransfer) {  // if create transferQueue
			if (presetQueues.transfer.family != std::numeric_limits<uint32_t>::max())  // if use user specified Info
				addUserQueueCreateInfo(transferQueue_m, presetQueues.transfer);
			else {
				auto onlyTransfer = std::find_if(gpu.queueFamilyProperties.begin(), gpu.queueFamilyProperties.end(), [](const VkQueueFamilyProperties & prop) {return prop.queueFlags == VK_QUEUE_TRANSFER_BIT; }); // look for transfer only queue
				if (onlyTransfer != gpu.queueFamilyProperties.end()) addQueueCreateInfo(transferQueue_m, static_cast<int>(std::distance(gpu.queueFamilyProperties.begin(), onlyTransfer)));
				else if (!gpu.queueFamilyTypes.transferFamilies.empty()) addQueueCreateInfo(transferQueue_m, gpu.queueFamilyTypes.transferFamilies[0]); // look for another tansferQueue 
				else if (!gpu.queueFamilyTypes.graphicFamilies.empty()) addQueueCreateInfo(transferQueue_m, gpu.queueFamilyTypes.graphicFamilies[0]);	// if non found, no queue will be created
				else if (!gpu.queueFamilyTypes.computeFamilies.empty()) addQueueCreateInfo(transferQueue_m, gpu.queueFamilyTypes.computeFamilies[0]);
			}
		}

		// Graphics and present
		if (presetQueues.createGraphics && presetQueues.createPresent) {  // if create present and create graphics
			if (presetQueues.graphics.family == std::numeric_limits<uint32_t>::max() && presetQueues.present.family == std::numeric_limits<uint32_t>::max()) { // and if do not use user input
				int presentIndex = -1;
				for (int i : gpu.queueFamilyTypes.graphicFamilies) {	 // look for family with graphics and present support
					VkBool32 presentSupport = checkPresentSupport(i);

					if (presentSupport) {
						presentIndex = i;
						break;
					}
				}

				if (presentIndex != -1) { // if common family found
					addQueueCreateInfo(graphicsQueue_m, presentIndex);
					addQueueCreateInfo(presentQueue_m, presentIndex);
				}
				else { // if not found use seperate families for graphics and present
					addQueueCreateInfo(graphicsQueue_m, gpu.queueFamilyTypes.graphicFamilies[0]);

					presentIndex = findPresentFamily();
					if (presentIndex != -1) {
						addQueueCreateInfo(presentQueue_m, presentIndex);
					}
				}
			}
			else { // if either present or graphics uses user input
				if (presetQueues.graphics.family != std::numeric_limits<uint32_t>::max()) {
					// if graphics uses user input
					addUserQueueCreateInfo(graphicsQueue_m, presetQueues.graphics);
					if (checkPresentSupport(presetQueues.graphics.family)) {
						addQueueCreateInfo(presentQueue_m, presetQueues.graphics.family);
					}
					else {
						if (checkPresentSupport(presetQueues.graphics.family)) {
							addQueueCreateInfo(presentQueue_m, presetQueues.graphics.family);
						}
						else {
							int i = findPresentFamily();
							if (i != -1) addQueueCreateInfo(presentQueue_m, i);
						}
					}
				}
				else { // if present uses use input
					addUserQueueCreateInfo(presentQueue_m, presetQueues.present);
					if (gpu.queueFamilyProperties[presetQueues.present.family].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
						addQueueCreateInfo(graphicsQueue_m, presetQueues.present.family);
					}
					else {
						if (std::find(gpu.queueFamilyTypes.graphicFamilies.begin(), gpu.queueFamilyTypes.graphicFamilies.end(), presetQueues.present.family) != gpu.queueFamilyTypes.graphicFamilies.end())
							addQueueCreateInfo(graphicsQueue_m, presetQueues.present.family);
						else 
							addQueueCreateInfo(graphicsQueue_m, gpu.queueFamilyTypes.graphicFamilies[0]);
					}
				}
			}
		}
		else if (presetQueues.createGraphics) { //if create graphics 
			if (presetQueues.graphics.family != std::numeric_limits<uint32_t>::max()) {
				addUserQueueCreateInfo(graphicsQueue_m, presetQueues.graphics);
			}
			else {
				addQueueCreateInfo(graphicsQueue_m, gpu.queueFamilyTypes.graphicFamilies[0]);
			}
		}
		else if (presetQueues.createPresent) { // else if create present
			if (presetQueues.graphics.family != std::numeric_limits<uint32_t>::max()) {
				addUserQueueCreateInfo(presentQueue_m, presetQueues.present);
			}
			else {
				int i = findPresentFamily();
				if (i != -1) addQueueCreateInfo(presentQueue_m, i);
			}
		}

		return createInfos;
	}

	std::vector<VkDeviceQueueCreateInfo> Device::setupQueueCreation(const CreateInfo & info, std::map<int, std::vector<float>> & priorities)
	{	
		std::vector<VkDeviceQueueCreateInfo> createInfos = setupPresetQueues(info.physicalDevice, info.preSetQueues, priorities, info.surfaces);

		for (auto & x : info.additionalQueues) {
			auto it = std::find_if(createInfos.begin(), createInfos.end(), [&](const VkDeviceQueueCreateInfo & info)->bool { return x.family == info.queueFamilyIndex && x.flags == info.flags; });

			if ( it != createInfos.end()) { // if found duplicate
				int sizeDif = (x.index + 1) - it->queueCount;

				if (sizeDif > 0) {	// if index is higher than queueCount (i.e more queues need to be created for index to be valid)
					for (int i = 0; i < sizeDif - 1; i++) priorities[x.family].push_back(INVALID_PRIORITY);
					priorities[x.family].insert(priorities[x.family].end(), x.priorities.begin(), x.priorities.end());
					it->pQueuePriorities = priorities[x.family].data();
				}
				else {
					if (x.index + 1 + x.priorities.size() > priorities[x.family].size())
						priorities[x.family].resize(x.index + 1 + x.priorities.size(), INVALID_PRIORITY);

					for (uint32_t i = 0; i < x.priorities.size(); i++) {
						if (priorities[x.family][x.index + i] == INVALID_PRIORITY) {
							priorities[x.family][x.index + i] = x.priorities[i];
						}
					}
				}

				it->queueCount = static_cast<uint32_t>(priorities[x.family].size());
				it->pQueuePriorities = priorities[x.family].data();
			}
			else {
				priorities[x.family] = x.priorities;
				VkDeviceQueueCreateInfo queueCreateInfo = init::deviceQueueCreateInfo();
				queueCreateInfo.queueFamilyIndex = x.family;
				queueCreateInfo.queueCount = static_cast<uint32_t>(priorities[x.family].size());
				queueCreateInfo.pQueuePriorities = priorities[x.family].data();
				queueCreateInfo.flags = x.flags;
				createInfos.push_back(queueCreateInfo);
			}
		}

		for (auto & x : priorities) for (float & p : x.second) if (p == INVALID_PRIORITY) p = 0.0; // getting rid of INVALID_PRIORITY
		return createInfos;
	}
}
