#pragma once
#include "vkw_Include.h"

#define VKW_DEFAULT_QUEUE -1

namespace vkw {
	class CommandBuffer;

	class CommandPool : public impl::Object<impl::VkwCommandPool> {
	public:
		struct CreateInfo : impl::CreateInfo {
			uint32_t queueFamily;
			VkCommandPoolCreateFlags flags = 0;
		};

		VULKAN_WRAPPER_API CommandPool() = default;
		VULKAN_WRAPPER_API CommandPool(const CreateInfo & createInfo);
		VULKAN_WRAPPER_API CommandPool(uint32_t queueFamily, VkCommandPoolCreateFlags flags = 0);
		VULKAN_WRAPPER_API ~CommandPool() = default;

		VULKAN_WRAPPER_API void createCommandPool(const CreateInfo & createInfo);
		VULKAN_WRAPPER_API void createCommandPool(uint32_t queueFamily, VkCommandPoolCreateFlags flags = 0);

		uint32_t queueFamily;
		VkCommandPoolCreateFlags flags = 0;
	};



	class CommandBuffer : public impl::Object<impl::VkwCommandBuffer> {
	public:
		struct AllocInfo : impl::CreateInfo{
			VkCommandPool commandPool;
			VkCommandBufferLevel level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		};

		VULKAN_WRAPPER_API static void allocateCommandBuffers(std::vector<CommandBuffer> & commandBuffers, VkCommandPool commandPool, VkCommandBufferLevel level = VK_COMMAND_BUFFER_LEVEL_PRIMARY);
		VULKAN_WRAPPER_API static void allocateCommandBuffers(std::vector<CommandBuffer> & commandBuffers, const AllocInfo & allocInfo);
		VULKAN_WRAPPER_API static void allocateCommandBuffers(std::vector<std::reference_wrapper<CommandBuffer>> commandBuffers, VkCommandPool commandPool, VkCommandBufferLevel level = VK_COMMAND_BUFFER_LEVEL_PRIMARY);
		VULKAN_WRAPPER_API static void allocateCommandBuffers(std::vector<std::reference_wrapper<CommandBuffer>> commandBuffers, const AllocInfo & allocInfo);
		//VULKAN_WRAPER_API static void freeCommandBuffers(std::vector<CommandBuffer> & commandBuffers);

		VULKAN_WRAPPER_API CommandBuffer();
		VULKAN_WRAPPER_API CommandBuffer(const AllocInfo & allocInfo);
		VULKAN_WRAPPER_API CommandBuffer(VkCommandPool commandPool, VkCommandBufferLevel level = VK_COMMAND_BUFFER_LEVEL_PRIMARY);
		VULKAN_WRAPPER_API ~CommandBuffer() = default;

		VULKAN_WRAPPER_API void allocateCommandBuffer(const AllocInfo & allocInfo);
		VULKAN_WRAPPER_API void allocateCommandBuffer(VkCommandPool commandPool, VkCommandBufferLevel level = VK_COMMAND_BUFFER_LEVEL_PRIMARY);

		VkCommandBufferLevel level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		const VkCommandPool & commandPool;

		VULKAN_WRAPPER_API void freeCommandBuffer();
		VULKAN_WRAPPER_API void beginCommandBuffer(VkCommandBufferUsageFlags flags = 0, VkCommandBufferInheritanceInfo * inheritanceInfo = nullptr);
		VULKAN_WRAPPER_API void endCommandBuffer();
		VULKAN_WRAPPER_API void resetCommandBuffer(VkCommandBufferResetFlags flags = 0);
		VULKAN_WRAPPER_API void submitCommandBuffer(VkQueue queue, std::vector<VkSemaphore> semaphore = {}, VkFence fence = VK_NULL_HANDLE);
	private:
		VkCommandPool commandPool_m;
	};





	class TransferCommandPool : public impl::Object<impl::VkwTransferCommandPool> {
	public:
		struct CreateInfo : impl::CreateInfo {
			int queueFamilyIndex = VKW_DEFAULT_QUEUE;
		};

		VULKAN_WRAPPER_API TransferCommandPool() = default;
		VULKAN_WRAPPER_API TransferCommandPool(int queueFamilyIndex);
		VULKAN_WRAPPER_API TransferCommandPool(const CreateInfo & createInfo);
		VULKAN_WRAPPER_API ~TransferCommandPool() = default;

		VULKAN_WRAPPER_API void createTransferCommandPool(int queueFamilyIndex = VKW_DEFAULT_QUEUE);
		VULKAN_WRAPPER_API void createTransferCommandPool(const CreateInfo & createInfo);
	};


	class GraphicsCommandPool : public impl::Object<impl::VkwGraphicsCommandPool> {
	public:
		struct CreateInfo : impl::CreateInfo {
			int queueFamilyIndex = VKW_DEFAULT_QUEUE;
		};

		VULKAN_WRAPPER_API GraphicsCommandPool() = default;
		VULKAN_WRAPPER_API GraphicsCommandPool(int queueFamilyIndex);
		VULKAN_WRAPPER_API GraphicsCommandPool(const CreateInfo & createInfo);
		VULKAN_WRAPPER_API ~GraphicsCommandPool() = default;

		VULKAN_WRAPPER_API void createGraphicsCommandPool(int queueFamilyIndex = VKW_DEFAULT_QUEUE);
		VULKAN_WRAPPER_API void createGraphicsCommandPool(const CreateInfo & createInfo);
	};


	class ComputeCommandPool : public impl::Object<impl::VkwComputeCommandPool> {
	public:
		struct CreateInfo : impl::CreateInfo {
			int queueFamilyIndex = VKW_DEFAULT_QUEUE;
		};

		VULKAN_WRAPPER_API ComputeCommandPool() = default;
		VULKAN_WRAPPER_API ComputeCommandPool(int queueFamilyIndex);
		VULKAN_WRAPPER_API ComputeCommandPool(const CreateInfo & createInfo);
		VULKAN_WRAPPER_API ~ComputeCommandPool() = default;

		VULKAN_WRAPPER_API void createComputeCommandPool(int queueFamilyIndex = VKW_DEFAULT_QUEUE);
		VULKAN_WRAPPER_API void createComputeCommandPool(const CreateInfo & createInfo);
	};
}