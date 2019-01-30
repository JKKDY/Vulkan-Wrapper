#include "Example.h"

namespace exp {

	Example::Example(GlfwWindow & window) :
		window(window)
	{
		setup();
		run();
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

		vkw::Instance instance{ instanceCreateInfo };

		if (instance.physicalDevices.size() == 0) {
			throw std::runtime_error("failed to find GPUs with Vulkan support!");
		}

		std::vector<vkw::PhysicalDevice> candidates;
		for (auto & x : instance.physicalDevices) {
			bool requiredQueueFamilySupport = x.queueFamilyTypes.graphicFamilies.size() * x.queueFamilyTypes.transferFamilies.size();
			

		}
		vkw::PhysicalDevice physicalDevice = instance.physicalDevices[0];
		vkw::Surface surface(window, physicalDevice);
	}

	void Example::run()
	{
	}
}

