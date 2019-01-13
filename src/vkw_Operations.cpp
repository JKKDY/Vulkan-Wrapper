#include "vkw_Operations.h"


namespace vkw {
	/// Command Pool
	CommandPool::CommandPool(uint32_t queueFamily, VkCommandPoolCreateFlags flags) :
		queueFamily(queueFamily), flags(flags)
	{
		createCommandPool();
	}

	void CommandPool::createCommandPool()
	{
		

		VkCommandPoolCreateInfo createInfo = Init::commandPoolCreateInfo();
		createInfo.flags = flags;
		createInfo.queueFamilyIndex = queueFamily;

		vkw::Debug::errorCodeCheck(vkCreateCommandPool(registry.device, &createInfo, nullptr, vkObject), "Failed to create Command Pool");
	}








	/// Command Buffers
	CommandBuffer::CommandBuffer(VkCommandPool commandPool, VkCommandBufferLevel level) :
		commandPool(commandPool), level(level)
	{
		destructionControl = impl::VKW_DESTR_CONTRL_DO_NOTHING;
		allocateCommandBuffer();
	}

	void CommandBuffer::destroyObject()
	{
		vkObject.destroyObject(destructionControl, [=](VkCommandBuffer obj) {vkFreeCommandBuffers(registry.device, this->commandPool, 1, &obj); });
	}

	void CommandBuffer::allocateCommandBuffer()
	{
		VkCommandBufferAllocateInfo allocInfo = Init::commandBufferAllocateInfo();
		allocInfo.level = level;
		allocInfo.commandPool = commandPool;
		allocInfo.commandBufferCount = 1;

		vkw::Debug::errorCodeCheck(vkAllocateCommandBuffers(registry.device, &allocInfo, vkObject), "Failed to allocate Command Buffer");
	}

	void CommandBuffer::freeCommandBuffer()
	{
		destroyObject();
	}

	void CommandBuffer::beginCommandBuffer(VkCommandBufferUsageFlags flags, VkCommandBufferInheritanceInfo * inheritanceInfo)
	{
		VkCommandBufferBeginInfo beginInfo = Init::commandBufferBeginInfo();
		beginInfo.flags = flags;
		beginInfo.pInheritanceInfo = inheritanceInfo;

		vkw::Debug::errorCodeCheck(vkBeginCommandBuffer(*vkObject, &beginInfo), "Failed to start recording of the Command Buffer");
	}

	void CommandBuffer::endCommandBuffer()
	{
		vkw::Debug::errorCodeCheck(vkEndCommandBuffer(*vkObject), "Failed to record command Buffer!");
	}

	void CommandBuffer::resetCommandBuffer(VkCommandBufferResetFlags flags) {
		vkw::Debug::errorCodeCheck(vkResetCommandBuffer(*vkObject, flags), "Failed to reset command Buffer!");
	}

	void CommandBuffer::submitCommandBuffer(VkQueue queue, std::vector<VkSemaphore> semaphore, VkFence fence)
	{
		VkSubmitInfo submitInfo = Init::submitInfo();
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = vkObject;
		submitInfo.signalSemaphoreCount = static_cast<uint32_t>(semaphore.size());
		submitInfo.pSignalSemaphores = semaphore.data();

		vkw::Debug::errorCodeCheck(vkQueueSubmit(queue, 1, &submitInfo, fence), "Failed to submit Command Buffer");
	}

	void CommandBuffer::allocateCommandBuffers(std::vector<CommandBuffer> & commandBuffers, VkCommandPool commandPool, VkCommandBufferLevel level)
	{
		std::vector<VkCommandBuffer> vkCommandBuffers(commandBuffers.size());

		VkCommandBufferAllocateInfo allocInfo = Init::commandBufferAllocateInfo();
		allocInfo.level = level;
		allocInfo.commandPool = commandPool;
		allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());

		vkw::Debug::errorCodeCheck(vkAllocateCommandBuffers(commandBuffers[0].registry.device, &allocInfo, vkCommandBuffers.data()), "Failed to allocate ommandBuffers");


		for (uint32_t i = 0; i < commandBuffers.size(); i++) {
			commandBuffers[i].commandPool = commandPool;
			//commandBuffers[i].
			commandBuffers[i].level = level;
			commandBuffers[i].vkObject = vkCommandBuffers[i];
		}
	}







	TransferCommandPool::TransferCommandPool()
	{
		//VKW_assert()
		create();
	}

	void TransferCommandPool::create()
	{
		VkCommandPoolCreateInfo createInfo = Init::commandPoolCreateInfo();
		createInfo.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;
		createInfo.queueFamilyIndex = registry.transferQueue.family;

		Debug::errorCodeCheck(vkCreateCommandPool(registry.device, &createInfo, nullptr, vkObject), "Failed to create Transfer Command Pool");
	}







	GraphicsCommandPool::GraphicsCommandPool()
	{
		if (registry.device != VK_NULL_HANDLE) {
			create();
		}
	}

	void GraphicsCommandPool::create()
	{
		VkCommandPoolCreateInfo createInfo = Init::commandPoolCreateInfo();
		createInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
		createInfo.queueFamilyIndex = registry.graphicsQueue.family;

		Debug::errorCodeCheck(vkCreateCommandPool(registry.device, &createInfo, nullptr, vkObject), "Failed to create Graphics Command Pool");
	}



	ComputeCommandPool::ComputeCommandPool()
	{
		if (registry.device != VK_NULL_HANDLE) {
			create();
		}
	}

	VULKAN_WRAPER_API void ComputeCommandPool::create()
	{
		VkCommandPoolCreateInfo createInfo = Init::commandPoolCreateInfo();
		createInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
		createInfo.queueFamilyIndex = registry.computeQueue.family;

		Debug::errorCodeCheck(vkCreateCommandPool(registry.device, &createInfo, nullptr, vkObject), "Failed to create Compute Command Pool");
	}
}

