#include "setup.hpp"


struct foo {
	~foo() {
		int i = 5;
	}
};


int main() {
	foo fff;

	GlfwWindow window = GlfwWindow(800, 600);

	VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo = vkw::init::debugUtilsMessengerCreateInfoEXT();
	debugCreateInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
		VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT |
		VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
		VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
	debugCreateInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
		VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
		VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
	debugCreateInfo.pfnUserCallback = debugCallback;
	debugCreateInfo.pUserData = nullptr;

	vkw::Instance::CreateInfo instanceCreateInfo = {};
	instanceCreateInfo.debugMessengerInfos = { debugCreateInfo };
	instanceCreateInfo.desiredExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
	instanceCreateInfo.desiredLayers.push_back("VK_LAYER_LUNARG_standard_validation");
	GlfwWindow::getWindowExtensions(instanceCreateInfo.desiredExtensions);

	std::vector<const char*> missingExtensions;
	instanceCreateInfo.desiredExtensions = vkw::Instance::checkExtensions(instanceCreateInfo.desiredExtensions, &missingExtensions);
	VKW_assert(missingExtensions.empty(), "Required Extensions missing");

	std::vector<const char*> missingLayers;
	instanceCreateInfo.desiredLayers = vkw::Instance::checkLayers(instanceCreateInfo.desiredLayers, &missingLayers);
	VKW_assert(missingLayers.empty(), "Required Layers missing");

	vkw::Instance instance(instanceCreateInfo);
	vkw::Surface surface(window);

	VKW_assert(instance.physicalDevices.size(), "failed to find GPUs with Vulkan support!");
	std::vector<vkw::PhysicalDevice> suitableDevices;
	for (auto & x : instance.physicalDevices) {
		bool familiesFound = false;
		for (uint32_t i = 0; i < x.queueFamilyProperties.size(); i++) {
			VkBool32 presentSupport = false;
			vkGetPhysicalDeviceSurfaceSupportKHR(x, i, surface, &presentSupport);
			if (presentSupport) familiesFound = true;
		}
		familiesFound = x.queueFamilyTypes.graphicFamilies.size() * x.queueFamilyTypes.transferFamilies.size();

		std::vector<const char*> missingLayers;
		x.checkLayers({ VK_KHR_SWAPCHAIN_EXTENSION_NAME }, &missingLayers);

		if (familiesFound * surface.formats(x).size() * surface.presentModes(x).size() * missingLayers.empty())
			suitableDevices.push_back(x);
	}
	std::map<int, vkw::PhysicalDevice*> candidates;
	for (auto &x : suitableDevices) {
		int score = 0;

		if (x.properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) score += 1000;
		score += x.properties.limits.maxImageDimension2D;

		for (int index : x.queueFamilyTypes.transferFamilies) {
			if (x.queueFamilyProperties.at(index).queueFlags == VK_QUEUE_TRANSFER_BIT) {
				score += 500;
				break;
			}
		}

		for (int index : x.queueFamilyTypes.graphicFamilies) {
			VkBool32 presentSupport = false;
			vkGetPhysicalDeviceSurfaceSupportKHR(x, index, surface, &presentSupport);
			if (presentSupport)  score += 500;
		}

		candidates.insert(std::make_pair(score, &x));
	}

	vkw::PhysicalDevice & physicalDevice = *candidates.rbegin()->second;

	vkw::Device::CreateInfo deviceCreateInfo = {};
	deviceCreateInfo.physicalDevice = physicalDevice;
	deviceCreateInfo.surfaces = { surface };
	deviceCreateInfo.extensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };

	vkw::Device device(deviceCreateInfo);
	vkw::Swapchain swapChain(surface);


	vkw::Buffer buffer1(VK_BUFFER_USAGE_INDEX_BUFFER_BIT, 1000);
	vkw::Buffer buffer2(VK_BUFFER_USAGE_INDEX_BUFFER_BIT, 2000);
	vkw::Memory memory(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
	memory.setMemoryTypeBitsBuffer(buffer1);
	memory.setMemoryTypeBitsBuffer(buffer2);
	memory.allocateMemory();
	memory.bindBufferToMemory(buffer1);
	memory.bindBufferToMemory(buffer2);

	auto m = &memory;
	auto r = &memory.memoryRanges_m;

	buffer1.destroyObject();

	buffer1.createBuffer(VK_BUFFER_USAGE_INDEX_BUFFER_BIT, 500);
	memory.bindBufferToMemory(buffer1);

	vkw::Buffer buffer3(VK_BUFFER_USAGE_INDEX_BUFFER_BIT, 1);
	memory.bindBufferToMemory(buffer3);

	vkw::Buffer buffer4(VK_BUFFER_USAGE_INDEX_BUFFER_BIT, 1);
	memory.bindBufferToMemory(buffer4);


	vkw::SubBuffer sub1 = buffer2.createSubBuffer(50);
	vkw::SubBuffer sub2 = buffer2.createSubBuffer(40);
	vkw::SubBuffer sub3;
	sub3 = buffer2.createSubBuffer(30);
	
	sub1.clear();
	
	vkw::SubBuffer sub4 = buffer2.createSubBuffer(10);

	
	int i = 0;
}
