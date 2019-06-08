#include "vkw_Operations.h"


namespace vkw {

	/// Command Pool
	CommandPool::CommandPool(const CreateInfo & createInfo) :
		CommandPool(createInfo.queueFamily, createInfo.flags)
	{}

	CommandPool::CommandPool(uint32_t queueFamily, VkCommandPoolCreateFlags flags)
	{
		createCommandPool(queueFamily, flags);
	}

	void CommandPool::createCommandPool(const CreateInfo & createInfo)
	{
		this->queueFamily = createInfo.queueFamily;
		this->flags = createInfo.flags;

		VkCommandPoolCreateInfo info = init::commandPoolCreateInfo();
		info.flags = flags;
		info.queueFamilyIndex = queueFamily;
		info.pNext = createInfo.pNext;

		vkw::Debug::errorCodeCheck(vkCreateCommandPool(registry.device, &info, nullptr, pVkObject.createNew()), "Failed to create Command Pool");
	}

	void CommandPool::createCommandPool(uint32_t queueFamily, VkCommandPoolCreateFlags flags)
	{
		CreateInfo createInfo = {};
		createInfo.queueFamily = queueFamily;
		createInfo.flags = flags;
		createCommandPool(createInfo);
	}






	/// Command Buffers
	void CommandBuffer::allocateCommandBuffers(std::vector<std::reference_wrapper<CommandBuffer>> commandBuffers, const AllocInfo & allocInfo)
	{
		CommandBuffer::allocateCommandBuffers(commandBuffers, allocInfo.commandPool, allocInfo.level);
	}

	void CommandBuffer::allocateCommandBuffers(std::vector<CommandBuffer> & commandBuffers, const AllocInfo & allocInfo)
	{
		CommandBuffer::allocateCommandBuffers(commandBuffers, allocInfo.commandPool, allocInfo.level);
	}

	void CommandBuffer::allocateCommandBuffers(std::vector<CommandBuffer> & commandBuffers, VkCommandPool commandPool, VkCommandBufferLevel level)
	{
		std::vector<VkCommandBuffer> vkCommandBuffers(commandBuffers.size());

		VkCommandBufferAllocateInfo allocInfo = init::commandBufferAllocateInfo();
		allocInfo.level = level;
		allocInfo.commandPool = commandPool;
		allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());

		if (commandBuffers.size()) vkw::Debug::errorCodeCheck(vkAllocateCommandBuffers(commandBuffers[0].registry.device, &allocInfo, vkCommandBuffers.data()), "Failed to allocate ommandBuffers");

		for (uint32_t i = 0; i < commandBuffers.size(); i++) {
			commandBuffers[i].commandPool_m = commandPool;
			commandBuffers[i].level = level;
			commandBuffers[i].pVkObject = vkCommandBuffers[i];
		}
	}

	void CommandBuffer::allocateCommandBuffers(std::vector<std::reference_wrapper<CommandBuffer>> commandBuffers, VkCommandPool commandPool, VkCommandBufferLevel level)
	{
		std::vector<VkCommandBuffer> vkCommandBuffers(commandBuffers.size());

		VkCommandBufferAllocateInfo allocInfo = init::commandBufferAllocateInfo();
		allocInfo.level = level;
		allocInfo.commandPool = commandPool;
		allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());

		if (commandBuffers.size()) vkw::Debug::errorCodeCheck(vkAllocateCommandBuffers(commandBuffers[0].get().registry.device, &allocInfo, vkCommandBuffers.data()), "Failed to allocate ommandBuffers");

		for (uint32_t i = 0; i < commandBuffers.size(); i++) {
			commandBuffers[i].get().commandPool_m = commandPool;
			commandBuffers[i].get().level = level;
			commandBuffers[i].get().pVkObject = vkCommandBuffers[i];
		}
	}

	CommandBuffer::CommandBuffer():
		Base([&](VkCommandBuffer obj) {
		if (commandPool != VK_NULL_HANDLE)
			vkFreeCommandBuffers(registry.device, commandPool_m, 1, &obj);
		}),
		commandPool(commandPool_m)
	{
		//destructionControl = VKW_DESTR_CONTRL_DO_NOTHING; // review if this should be set or not
	}

	CommandBuffer::CommandBuffer(const AllocInfo & allocInfo):
		CommandBuffer(allocInfo.commandPool, allocInfo.level)
	{
	}

	CommandBuffer::CommandBuffer(VkCommandPool commandPool, VkCommandBufferLevel level) :
		CommandBuffer()
	{
		allocateCommandBuffer(commandPool, level);
	}

	void CommandBuffer::allocateCommandBuffer(const AllocInfo & allocInfo)
	{
		this->level = level;
		this->commandPool_m = allocInfo.commandPool;

		VkCommandBufferAllocateInfo info = init::commandBufferAllocateInfo();
		info.level = level;
		info.commandPool = allocInfo.commandPool;
		info.commandBufferCount = 1;
		info.pNext = allocInfo.pNext;

		vkw::Debug::errorCodeCheck(vkAllocateCommandBuffers(registry.device, &info, pVkObject.createNew()), "Failed to allocate Command Buffer");
	}

	void CommandBuffer::allocateCommandBuffer(VkCommandPool commandPool, VkCommandBufferLevel level)
	{
		AllocInfo allocInfo = {};
		allocInfo.commandPool = commandPool;
		allocInfo.level = level;
		allocateCommandBuffer(allocInfo);
	}

	void CommandBuffer::freeCommandBuffer()
	{
		pVkObject.destroyObject();
	}

	void CommandBuffer::beginCommandBuffer(VkCommandBufferUsageFlags flags, VkCommandBufferInheritanceInfo * inheritanceInfo)
	{
		VkCommandBufferBeginInfo beginInfo = init::commandBufferBeginInfo();
		beginInfo.flags = flags;
		beginInfo.pInheritanceInfo = inheritanceInfo;

		vkw::Debug::errorCodeCheck(vkBeginCommandBuffer(*pVkObject, &beginInfo), "Failed to start recording of the Command Buffer");
	}

	void CommandBuffer::endCommandBuffer()
	{
		vkw::Debug::errorCodeCheck(vkEndCommandBuffer(*pVkObject), "Failed to record command Buffer!");
	}

	void CommandBuffer::resetCommandBuffer(VkCommandBufferResetFlags flags) {
		vkw::Debug::errorCodeCheck(vkResetCommandBuffer(*pVkObject, flags), "Failed to reset command Buffer!");
	}

	void CommandBuffer::submitCommandBuffer(VkQueue queue, std::vector<VkSemaphore> semaphore, VkFence fence)
	{
		VkSubmitInfo submitInfo = init::submitInfo();
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = pVkObject;
		submitInfo.signalSemaphoreCount = static_cast<uint32_t>(semaphore.size());
		submitInfo.pSignalSemaphores = semaphore.data();

		vkw::Debug::errorCodeCheck(vkQueueSubmit(queue, 1, &submitInfo, fence), "Failed to submit Command Buffer");
	}





	/// TranferCimmandPool
	TransferCommandPool::TransferCommandPool(int queueFamilyIndex)
	{
		createTransferCommandPool(queueFamilyIndex);
	}

	TransferCommandPool::TransferCommandPool(const CreateInfo & createInfo):
		TransferCommandPool()
	{}

	void TransferCommandPool::createTransferCommandPool(const CreateInfo & createInfo)
	{
		VkCommandPoolCreateInfo info = init::commandPoolCreateInfo();
		info.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;
		info.queueFamilyIndex = createInfo.queueFamilyIndex == VKW_DEFAULT_QUEUE ? registry.transferQueue.family : createInfo.queueFamilyIndex;
		info.pNext = info.pNext;

		Debug::errorCodeCheck(vkCreateCommandPool(registry.device, &info, nullptr, pVkObject.createNew()), "Failed to create Transfer Command Pool");
	}

	void TransferCommandPool::createTransferCommandPool(int queueFamilyIndex)
	{
		CreateInfo createInfo = {};
		createInfo.queueFamilyIndex = queueFamilyIndex;
		createTransferCommandPool(createInfo);
	}



	/// GraphicsCommandPool
	GraphicsCommandPool::GraphicsCommandPool(int queueFamilyIndex)
	{
		createGraphicsCommandPool(queueFamilyIndex);
	}

	GraphicsCommandPool::GraphicsCommandPool(const CreateInfo & createInfo):
		GraphicsCommandPool(createInfo.queueFamilyIndex)
	{}

	void GraphicsCommandPool::createGraphicsCommandPool(const CreateInfo & createInfo)
	{
		VkCommandPoolCreateInfo info = init::commandPoolCreateInfo();
		info.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;
		info.queueFamilyIndex = createInfo.queueFamilyIndex == VKW_DEFAULT_QUEUE ? registry.graphicsQueue.family : createInfo.queueFamilyIndex;
		info.pNext = info.pNext;

		Debug::errorCodeCheck(vkCreateCommandPool(registry.device, &info, nullptr, pVkObject.createNew()), "Failed to create Transfer Command Pool");
	}

	void GraphicsCommandPool::createGraphicsCommandPool(int queueFamilyIndex)
	{
		CreateInfo createInfo = {};
		createInfo.queueFamilyIndex = queueFamilyIndex;
		createGraphicsCommandPool(createInfo);
	}



	/// ComputeCommandPool
	ComputeCommandPool::ComputeCommandPool(int queueFamilyIndex)
	{
		createComputeCommandPool(queueFamilyIndex);
	}

	ComputeCommandPool::ComputeCommandPool(const CreateInfo & createInfo):
		ComputeCommandPool(createInfo.queueFamilyIndex)
	{}

	void ComputeCommandPool::createComputeCommandPool(const CreateInfo & createInfo)
	{
		VkCommandPoolCreateInfo info = init::commandPoolCreateInfo();
		info.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;
		info.queueFamilyIndex = createInfo.queueFamilyIndex == VKW_DEFAULT_QUEUE ? registry.computeQueue.family : createInfo.queueFamilyIndex;
		info.pNext = info.pNext;

		Debug::errorCodeCheck(vkCreateCommandPool(registry.device, &info, nullptr, pVkObject.createNew()), "Failed to create Transfer Command Pool");
	}

	void ComputeCommandPool::createComputeCommandPool(int queueFamilyIndex)
	{
		CreateInfo createInfo = {};
		createInfo.queueFamilyIndex = queueFamilyIndex;
		createComputeCommandPool(createInfo);
	}

}

