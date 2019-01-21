#pragma once
#include "vkw_Include.h"


namespace vkw {
	class CommandBuffer;

	class CommandPool : public impl::Entity<impl::VkwCommandPool> {
	public:
		struct CreateInfo {
			uint32_t queueFamily;
			VkCommandPoolCreateFlags flags = 0;
		};

		VULKAN_WRAPER_API CommandPool() = default;
		VULKAN_WRAPER_API CommandPool(const CreateInfo & createInfo);
		VULKAN_WRAPER_API CommandPool(uint32_t queueFamily, VkCommandPoolCreateFlags flags = 0);
		VULKAN_WRAPER_API ~CommandPool() = default;

		VULKAN_WRAPER_API void createCommandPool(const CreateInfo & createInfo);
		VULKAN_WRAPER_API void createCommandPool(uint32_t queueFamily, VkCommandPoolCreateFlags flags = 0);

		uint32_t queueFamily;
		VkCommandPoolCreateFlags flags = 0;
	};



	class CommandBuffer : public impl::Entity<impl::VkwCommandBuffer> {
	public:
		struct AllocationInfo {
			VkCommandPool commandPool;
			VkCommandBufferLevel level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		};

		VULKAN_WRAPER_API static void allocateCommandBuffers(std::vector<CommandBuffer> & commandBuffers, VkCommandPool commandPool, VkCommandBufferLevel level = VK_COMMAND_BUFFER_LEVEL_PRIMARY);
		VULKAN_WRAPER_API static void allocateCommandBuffers(std::vector<CommandBuffer> & commandBuffers, const AllocationInfo & allocInfo);
		VULKAN_WRAPER_API static void allocateCommandBuffers(std::vector<std::reference_wrapper<CommandBuffer>> commandBuffers, VkCommandPool commandPool, VkCommandBufferLevel level = VK_COMMAND_BUFFER_LEVEL_PRIMARY);
		VULKAN_WRAPER_API static void allocateCommandBuffers(std::vector<std::reference_wrapper<CommandBuffer>> commandBuffers, const AllocationInfo & allocInfo);
		//VULKAN_WRAPER_API static void freeCommandBuffers(std::vector<CommandBuffer> & commandBuffers);

		VULKAN_WRAPER_API CommandBuffer();
		VULKAN_WRAPER_API CommandBuffer(const AllocationInfo & allocInfo);
		VULKAN_WRAPER_API CommandBuffer(VkCommandPool commandPool, VkCommandBufferLevel level = VK_COMMAND_BUFFER_LEVEL_PRIMARY);
		VULKAN_WRAPER_API ~CommandBuffer() = default;

		VULKAN_WRAPER_API void allocateCommandBuffer(const AllocationInfo & allocInfo);
		VULKAN_WRAPER_API void allocateCommandBuffer(VkCommandPool commandPool, VkCommandBufferLevel level = VK_COMMAND_BUFFER_LEVEL_PRIMARY);

		VkCommandBufferLevel level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		VkCommandPool commandPool;

		VULKAN_WRAPER_API void destroyObject() override;
		VULKAN_WRAPER_API void freeCommandBuffer();
		VULKAN_WRAPER_API void beginCommandBuffer(VkCommandBufferUsageFlags flags = 0, VkCommandBufferInheritanceInfo * inheritanceInfo = nullptr);
		VULKAN_WRAPER_API void endCommandBuffer();
		VULKAN_WRAPER_API void resetCommandBuffer(VkCommandBufferResetFlags flags = 0);
		VULKAN_WRAPER_API void submitCommandBuffer(VkQueue queue, std::vector<VkSemaphore> semaphore = {}, VkFence fence = VK_NULL_HANDLE);
	};

#define VKW_DEFAULT_QUEUE -1

	class TransferCommandPool : public impl::Entity<impl::VkwTransferCommandPool> {
	public:
		struct CreateInfo {
			int queueFamilyIndex = VKW_DEFAULT_QUEUE;
		};

		VULKAN_WRAPER_API TransferCommandPool() = default;
		VULKAN_WRAPER_API TransferCommandPool(int queueFamilyIndex);
		VULKAN_WRAPER_API TransferCommandPool(const CreateInfo & createInfo);
		VULKAN_WRAPER_API ~TransferCommandPool() = default;

		VULKAN_WRAPER_API void create(int queueFamilyIndex = -1);
		VULKAN_WRAPER_API void create(const CreateInfo & createInfo);
	};


	class GraphicsCommandPool : public impl::Entity<impl::VkwGraphicsCommandPool> {
	public:
		struct CreateInfo {
			int queueFamilyIndex = VKW_DEFAULT_QUEUE;
		};

		VULKAN_WRAPER_API GraphicsCommandPool() = default;
		VULKAN_WRAPER_API GraphicsCommandPool(int queueFamilyIndex);
		VULKAN_WRAPER_API GraphicsCommandPool(const CreateInfo & createInfo);
		VULKAN_WRAPER_API ~GraphicsCommandPool() = default;

		VULKAN_WRAPER_API void create(int queueFamilyIndex = -1);
		VULKAN_WRAPER_API void create(const CreateInfo & createInfo);
	};


	class ComputeCommandPool : public impl::Entity<impl::VkwComputeCommandPool> {
	public:
		struct CreateInfo {
			int queueFamilyIndex = VKW_DEFAULT_QUEUE;
		};

		VULKAN_WRAPER_API ComputeCommandPool() = default;
		VULKAN_WRAPER_API ComputeCommandPool(int queueFamilyIndex);
		VULKAN_WRAPER_API ComputeCommandPool(const CreateInfo & createInfo);
		VULKAN_WRAPER_API ~ComputeCommandPool() = default;

		VULKAN_WRAPER_API void create(int queueFamilyIndex = -1);
		VULKAN_WRAPER_API void create(const CreateInfo & createInfo);
	};
}