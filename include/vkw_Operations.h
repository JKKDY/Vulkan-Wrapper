#pragma once
#include "vkw_Include.h"


namespace vkw {
	class CommandBuffer;

	class CommandPool : public impl::Entity<impl::VkwCommandPool> {
	public:
		VULKAN_WRAPER_API CommandPool() = default;
		VULKAN_WRAPER_API CommandPool(uint32_t queueFamily, VkCommandPoolCreateFlags flags = 0);
		VULKAN_WRAPER_API ~CommandPool() = default;

		uint32_t queueFamily;
		VkCommandPoolCreateFlags flags = 0;
		void createCommandPool();
	};



	class CommandBuffer : public impl::Entity<impl::VkwCommandBuffer> {
	public:
		VULKAN_WRAPER_API static void allocateCommandBuffers(std::vector<CommandBuffer> & commandBuffers, VkCommandPool commandPool, VkCommandBufferLevel level = VK_COMMAND_BUFFER_LEVEL_PRIMARY);
		//VULKAN_WRAPER_API static void freeCommandBuffers(std::vector<CommandBuffer> & commandBuffers);

		VULKAN_WRAPER_API CommandBuffer() = default;
		VULKAN_WRAPER_API CommandBuffer(VkCommandPool commandPool, VkCommandBufferLevel level = VK_COMMAND_BUFFER_LEVEL_PRIMARY);
		VULKAN_WRAPER_API ~CommandBuffer() = default;

		VkCommandBufferLevel level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		VkCommandPool commandPool;

		VULKAN_WRAPER_API void destroyObject() override;
		VULKAN_WRAPER_API void allocateCommandBuffer();
		VULKAN_WRAPER_API void freeCommandBuffer();
		VULKAN_WRAPER_API void beginCommandBuffer(VkCommandBufferUsageFlags flags = 0, VkCommandBufferInheritanceInfo * inheritanceInfo = nullptr);
		VULKAN_WRAPER_API void endCommandBuffer();
		VULKAN_WRAPER_API void resetCommandBuffer(VkCommandBufferResetFlags flags = 0);
		VULKAN_WRAPER_API void submitCommandBuffer(VkQueue queue, std::vector<VkSemaphore> semaphore = {}, VkFence fence = VK_NULL_HANDLE);
	};



	class TransferCommandPool : public impl::Entity<impl::VkwTransferCommandPool> {
	public:
		VULKAN_WRAPER_API TransferCommandPool();
		VULKAN_WRAPER_API ~TransferCommandPool() = default;
		VULKAN_WRAPER_API void create();
	};


	class GraphicsCommandPool : public impl::Entity<impl::VkwGraphicsCommandPool> {
	public:
		VULKAN_WRAPER_API GraphicsCommandPool();
		VULKAN_WRAPER_API ~GraphicsCommandPool() = default;
		VULKAN_WRAPER_API void create();
	};


	class ComputeCommandPool : public impl::Entity<impl::VkwComputeCommandPool> {
	public:
		VULKAN_WRAPER_API ComputeCommandPool();
		VULKAN_WRAPER_API ~ComputeCommandPool() = default;
		VULKAN_WRAPER_API void create();
	};
}