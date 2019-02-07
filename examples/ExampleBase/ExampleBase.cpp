#include "ExampleBase.h"

Example::Example(vkw::Window & window):
	window(window)
{
}

void Example::setup()
{
	setupCore();
}

void Example::setupCore()
{
	vkw::Instance::CreateInfo instanceCreateInfo = {};
	instanceCreateInfo.applicationName = "Example";
	instanceCreateInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	GlfwWindow::getWindowExtensions(instanceCreateInfo.desiredExtensions);
}

bool Example::deviceSuitable(const vkw::PhysicalDevice & gpu, const vkw::Surface surface)
{
	bool familiesFound = false;
	for (uint32_t i = 0; i < gpu.queueFamilyProperties.size(); i++) {
		VkBool32 presentSupport = false;
		vkGetPhysicalDeviceSurfaceSupportKHR(gpu, i, surface, &presentSupport);
		if (presentSupport) familiesFound = true;
	}
	familiesFound = gpu.queueFamilyTypes.graphicFamilies.size() * gpu.queueFamilyTypes.transferFamilies.size();

	if (familiesFound * surface.formats(gpu).size() * surface.presentModes(gpu).size())
		return true;
	else
		return false;
}

int Example::rateDevice(const vkw::PhysicalDevice & gpu, const vkw::Surface surface)
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
