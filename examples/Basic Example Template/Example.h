#pragma once
#include "ExampleBase.h"



#include <vulkan_wrapper.h>
#include "Window.hpp"


namespace exp {
	class Example {
	public:
		Example(GlfwWindow & window);
		~Example();

		

	protected:
		GlfwWindow & window;

		vkw::Instance instance;
		vkw::PhysicalDevice physicalDevice;
		vkw::Device device;
		vkw::Surface surface;
		vkw::Swapchain swapchain;

		vkw::TransferCommandPool transferCommandPool;
		vkw::GraphicsCommandPool graphicsCommandPool;

		vkw::RenderPass drawRenderPass;
		vkw::Swapchain swapchain;

		std::vector<vkw::FrameBuffer> drawFramebuffers;
		std::vector<vkw::CommandBuffer> drawCommandBufffers;

		void setup();
		void setupCore();
		void run();
	};
}