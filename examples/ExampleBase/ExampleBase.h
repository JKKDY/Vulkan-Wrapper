#pragma once
#include <vulkan_wrapper.h>



class Example{
public:
	struct ExampleInfo {
		std::reference_wrapper<vkw::Window> & window;
		std::vector<const char*> extensions;
	};
	Example(const ExampleInfo & info);
private:
	vkw::Window & window;

	vkw::Instance instance;
	vkw::Device device;
	vkw::PhysicalDevice physicalDevice;
	vkw::Swapchain swapchain;

	vkw::TransferCommandPool transferCommandPool;
	vkw::GraphicsCommandPool graphicsCommandPool;

	virtual void setup();
	void setupCore();

	virtual bool deviceSuitable(const vkw::PhysicalDevice & gpu, const vkw::Surface surface);
	virtual int rateDevice(const vkw::PhysicalDevice & gpu, const vkw::Surface surface);
};