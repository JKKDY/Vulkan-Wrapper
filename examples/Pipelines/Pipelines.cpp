#include "ExampleBase.h"
#include "Window.hpp"

std::string dataPath = "../data/";
std::string modelPath = dataPath + "Models/";
std::string texturePath = dataPath + "Textures/";
std::string shaderPath = dataPath + "Shader/";

struct foo {
	~foo() {
		int i = 5;
	}
};


int main() {
	foo f;

	// glfw test
	GlfwWindow window = GlfwWindow(1000, 600);

	Example example(window);

	/// Core
	vkw::Instance::CreateInfo instanceCreateInfo = {};
	instanceCreateInfo.applicationName = "Pipeline";
	instanceCreateInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	GlfwWindow::getWindowExtensions(instanceCreateInfo.desiredExtensions);

	std::vector<const char*> missingExtensions;
	instanceCreateInfo.desiredExtensions = vkw::Instance::checkExtensions(instanceCreateInfo.desiredExtensions, &missingExtensions);
	VKW_assert(missingExtensions.empty(), "Required Extensions missing");

	vkw::Instance instance(instanceCreateInfo);
	vkw::Surface surface(window);

	VKW_assert(instance.physicalDevices.size(), "failed to find GPUs with Vulkan support!");

	std::vector<vkw::PhysicalDevice> suitableDevices;

	//check for suitability
	for (auto & x : instance.physicalDevices) {
		bool familiesFound = false;
		for (uint32_t i = 0; i < x.queueFamilyProperties.size(); i++) {
			VkBool32 presentSupport = false;
			vkGetPhysicalDeviceSurfaceSupportKHR(x, i, surface, &presentSupport);
			if (presentSupport) familiesFound = true;
		}
		familiesFound = x.queueFamilyTypes.graphicFamilies.size() * x.queueFamilyTypes.transferFamilies.size();

		if (familiesFound * surface.formats(x).size() * surface.presentModes(x).size()) suitableDevices.push_back(x);
	}

	//select best device
	std::map<int, vkw::PhysicalDevice*> candidates;
	for (auto &x : suitableDevices) {
		int score = 0;

		if (x.properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) score += 1000;
		score += x.properties.limits.maxImageDimension2D;  //affects Image Quality

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

	vkw::Device device(deviceCreateInfo);
	vkw::Swapchain swapChain(surface);

	vkw::TransferCommandPool transferCommandPool(VKW_DEFAULT_QUEUE);
	vkw::GraphicsCommandPool graphicsCommandPool(VKW_DEFAULT_QUEUE);
	vkw::ComputeCommandPool computeCommandPool(VKW_DEFAULT_QUEUE);
}