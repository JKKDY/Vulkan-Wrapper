#pragma once
#include "ExampleBase.h"



#include <vulkan_wraper.h>
#include "Window.hpp"
namespace exaxmple {
	class Example {
	public:
		Example();

		~Example() {
			vkDeviceWaitIdle(device);
		}

		virtual void setupExample() {
			setupCore();
			setupRessources();
		}

		void setupCore() {
			vkw::Instance::CreateInfo instanceCreateInfo = {};
			instanceCreateInfo.applicationName = "Triangle";
			instanceCreateInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
			GlfwWindow::getWindowExtensions(instanceCreateInfo.desiredExtensions);

			instanceCreateInfo.desiredExtensions = vkw::Instance::checkExtensions(instanceCreateInfo.desiredExtensions);

			vkw::Device::CreateInfo deviceCreateInfo = {};
			deviceCreateInfo.features.multiViewport = VK_TRUE;
			deviceCreateInfo.features.samplerAnisotropy = VK_TRUE;
			deviceCreateInfo.features.fillModeNonSolid = VK_TRUE;


			/// Core
			instance = vkw::Instance(instanceCreateInfo);


		}

		virtual void setupRessources();

		virtual void run() {
			vkw::Semaphore semaphore = vkw::Semaphore();
			vkw::Fence fence = vkw::Fence();

			while (!glfwWindowShouldClose(window)) {

				uint32_t index = swapchain.getNextImage(VK_NULL_HANDLE, fence);
				fence.wait();
				drawCommandBufffers[index].submitCommandBuffer(vkw::Info::deviceQueuesInUse->graphicsQueue, { semaphore });
				swapchain.presentImage(index, { semaphore });

				glfwPollEvents();
			}
		}

	protected:
		GlfwWindow window = GlfwWindow(800, 600);

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
	};
}