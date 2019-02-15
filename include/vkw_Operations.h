#pragma once
#include "vkw_Include.h"

#define VKW_DEFAULT_QUEUE -1

namespace vkw {
	class CommandBuffer;

	class CommandPool : public impl::Entity<impl::VkwCommandPool> {
	public:
		struct CreateInfo {
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



	class CommandBuffer : public impl::Entity<impl::VkwCommandBuffer> {
	public:
		struct AllocationInfo {
			VkCommandPool commandPool;
			VkCommandBufferLevel level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		};

		VULKAN_WRAPPER_API static void allocateCommandBuffers(std::vector<CommandBuffer> & commandBuffers, VkCommandPool commandPool, VkCommandBufferLevel level = VK_COMMAND_BUFFER_LEVEL_PRIMARY);
		VULKAN_WRAPPER_API static void allocateCommandBuffers(std::vector<CommandBuffer> & commandBuffers, const AllocationInfo & allocInfo);
		VULKAN_WRAPPER_API static void allocateCommandBuffers(std::vector<std::reference_wrapper<CommandBuffer>> commandBuffers, VkCommandPool commandPool, VkCommandBufferLevel level = VK_COMMAND_BUFFER_LEVEL_PRIMARY);
		VULKAN_WRAPPER_API static void allocateCommandBuffers(std::vector<std::reference_wrapper<CommandBuffer>> commandBuffers, const AllocationInfo & allocInfo);
		//VULKAN_WRAPER_API static void freeCommandBuffers(std::vector<CommandBuffer> & commandBuffers);

		VULKAN_WRAPPER_API CommandBuffer();
		VULKAN_WRAPPER_API CommandBuffer(const AllocationInfo & allocInfo);
		VULKAN_WRAPPER_API CommandBuffer(VkCommandPool commandPool, VkCommandBufferLevel level = VK_COMMAND_BUFFER_LEVEL_PRIMARY);
		VULKAN_WRAPPER_API ~CommandBuffer() = default;

		VULKAN_WRAPPER_API void allocateCommandBuffer(const AllocationInfo & allocInfo);
		VULKAN_WRAPPER_API void allocateCommandBuffer(VkCommandPool commandPool, VkCommandBufferLevel level = VK_COMMAND_BUFFER_LEVEL_PRIMARY);

		VkCommandBufferLevel level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		const VkCommandPool & commandPool;

		VULKAN_WRAPPER_API void destroyObject() override;
		VULKAN_WRAPPER_API void freeCommandBuffer();
		VULKAN_WRAPPER_API void beginCommandBuffer(VkCommandBufferUsageFlags flags = 0, VkCommandBufferInheritanceInfo * inheritanceInfo = nullptr);
		VULKAN_WRAPPER_API void endCommandBuffer();
		VULKAN_WRAPPER_API void resetCommandBuffer(VkCommandBufferResetFlags flags = 0);
		VULKAN_WRAPPER_API void submitCommandBuffer(VkQueue queue, std::vector<VkSemaphore> semaphore = {}, VkFence fence = VK_NULL_HANDLE);
	private:
		VkCommandPool commandPool_m;
	};





	class TransferCommandPool : public impl::Entity<impl::VkwTransferCommandPool> {
	public:
		struct CreateInfo {
			int queueFamilyIndex = VKW_DEFAULT_QUEUE;
		};

		VULKAN_WRAPPER_API TransferCommandPool() = default;
		VULKAN_WRAPPER_API TransferCommandPool(int queueFamilyIndex);
		VULKAN_WRAPPER_API TransferCommandPool(const CreateInfo & createInfo);
		VULKAN_WRAPPER_API ~TransferCommandPool() = default;

		VULKAN_WRAPPER_API void create(int queueFamilyIndex = -1);
		VULKAN_WRAPPER_API void create(const CreateInfo & createInfo);
	};


	class GraphicsCommandPool : public impl::Entity<impl::VkwGraphicsCommandPool> {
	public:
		struct CreateInfo {
			int queueFamilyIndex = VKW_DEFAULT_QUEUE;
		};

		VULKAN_WRAPPER_API GraphicsCommandPool() = default;
		VULKAN_WRAPPER_API GraphicsCommandPool(int queueFamilyIndex);
		VULKAN_WRAPPER_API GraphicsCommandPool(const CreateInfo & createInfo);
		VULKAN_WRAPPER_API ~GraphicsCommandPool() = default;

		VULKAN_WRAPPER_API void create(int queueFamilyIndex = -1);
		VULKAN_WRAPPER_API void create(const CreateInfo & createInfo);
	};


	class ComputeCommandPool : public impl::Entity<impl::VkwComputeCommandPool> {
	public:
		struct CreateInfo {
			int queueFamilyIndex = VKW_DEFAULT_QUEUE;
		};

		VULKAN_WRAPPER_API ComputeCommandPool() = default;
		VULKAN_WRAPPER_API ComputeCommandPool(int queueFamilyIndex);
		VULKAN_WRAPPER_API ComputeCommandPool(const CreateInfo & createInfo);
		VULKAN_WRAPPER_API ~ComputeCommandPool() = default;

		VULKAN_WRAPPER_API void create(int queueFamilyIndex = -1);
		VULKAN_WRAPPER_API void create(const CreateInfo & createInfo);
	};
}