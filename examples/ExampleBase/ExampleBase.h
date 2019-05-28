#pragma once
#include <vulkan_wrapper.h>
#include <functional>
#include <chrono>
#include <thread>
#include "Model.h"
#include "Texture.h"
#include "Scene.h"
#include "Window.hpp"

namespace vkx {
	std::string getAssetPath();
	std::string modelPath();
	std::string texturePath();
	std::string shaderPath();

	void sleep(uint32_t ms);

	bool defaultDeviceSuitable(const vkw::PhysicalDevice & gpu, const vkw::Surface surface);
	int defaultRateDevice(const vkw::PhysicalDevice & gpu, const vkw::Surface surface);

	VKAPI_ATTR VkBool32 VKAPI_CALL defaultDebugCallback(
		VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
		VkDebugUtilsMessageTypeFlagsEXT messageType,
		const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
		void* pUserData);

	struct InitInfo {
		const std::vector<const char*> & instanceExtensions = {};
		const std::vector<const char*> & instanceLayers = {};
		VkApplicationInfo * appInfo = nullptr;
		std::vector<std::function<bool(const vkw::PhysicalDevice&, const vkw::Surface&)>> deviceSuitableFkt = { defaultDeviceSuitable };
		std::vector<std::function<int(const vkw::PhysicalDevice&, const vkw::Surface&)>> rateDevicefkt = { defaultRateDevice };
		vkw::Device::PreSetQueuesCreateInfo preSetDeviceQueues;
		std::vector<vkw::Device::AdditionalQueueCreateInfo> additionalDeviceQueues = {};
		VkPhysicalDeviceFeatures deviceFeatures = {};
		std::vector<const char*> deviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
	};

	class ExampleBase {
	public:
		ExampleBase(Window & window);
		~ExampleBase();
		void initVulkan(const InitInfo & info = InitInfo());

		virtual void setup() = 0;
		virtual void nextFrame();
	protected:
		Camera camera;

		float frameTimer = 0; // in seconds
		float time = 0; // in seconds

		Window & window;

		vkw::Instance instance;
		vkw::Surface surface;
		vkw::PhysicalDevice physicalDevice;
		vkw::Device device;

		vkw::TransferCommandPool transferCommandPool;
		vkw::GraphicsCommandPool graphicsCommandPool;

		vkw::Swapchain swapChain;
		vkw::RenderPass renderPass;

		vkw::Memory depthStencilMemory;
		vkw::Image depthImage;
		vkw::ImageView depthImageView;
		
		std::vector<vkw::FrameBuffer> renderFrameBuffers;
		std::vector<vkw::CommandBuffer> drawCommandBuffers;
		vkw::Semaphore renderSemaphore;
		vkw::Fence renderFence;

		vkw::PipelineCache pipelineCache;

		MeshLoader meshLoader;

		virtual void createInstance(const std::vector<const char*> & extensions, const std::vector<const char*> & layers, VkApplicationInfo * appInfo);
		virtual void createSurface();
		virtual void choosePhysicalDevice(std::vector<std::function<bool(const vkw::PhysicalDevice&, const vkw::Surface&)>> deviceSuitableFkt, std::vector<std::function<int(const vkw::PhysicalDevice&, const vkw::Surface&)>> rateDevicefkt);
		virtual void createDevice(vkw::Device::PreSetQueuesCreateInfo preSetDeviceQueues, std::vector<vkw::Device::AdditionalQueueCreateInfo> additionalDeviceQueues, VkPhysicalDeviceFeatures deviceFeatures, std::vector<const char*> deviceExtensions);
		virtual void createSwapchain();
		virtual void createDefaultCmdPools();

		virtual void createRenderPrimitives();
		virtual void createRenderPass();
		virtual void createDepthStencil();
		virtual void createPipelineCache();
		virtual void createFrameBuffers();
		virtual void allocateCommandBuffers();

		virtual void renderFrame();
	};
}
