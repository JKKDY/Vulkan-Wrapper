#include "vkw_Resources.h"

#include "vkw_Operations.h"
#include "vkw_Assets.h"
#include <math.h>


namespace vkw {

	/// Descriptor Pool
	uint32_t DescriptorPool::CreateInfo2::operator()(int descr) const
	{
		switch (descr) {
		case VK_DESCRIPTOR_TYPE_SAMPLER:
			return samplerCount;
		case VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER:
			return combinedImageSamplerCount;
		case VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE:
			return sampledImageCount;
		case VK_DESCRIPTOR_TYPE_STORAGE_IMAGE:
			return storageImageCount;
		case VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER:
			return uniformBufferCount;
		case VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER:
			return storageTexelBufferCount;
		case VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER:
			return uniformBufferCount;
		case VK_DESCRIPTOR_TYPE_STORAGE_BUFFER:
			return storageBufferCount;
		case VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC:
			return uniformBufferCount;
		case VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC:
			return storageBufferCount;
		case VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT:
			return inputAttachementCount;
		case VK_DESCRIPTOR_TYPE_INLINE_UNIFORM_BLOCK_EXT:
			return inlineUniformBlockEXTCount;
		case VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_NV:
			return accelerationStructureNVCount;
		}

		return 0;
	}

	DescriptorPool::DescriptorPool() :
		poolSizes(poolSizes_m),
		maxSets(maxSets_m),
		flags(flags_m)
	{}

	DescriptorPool::DescriptorPool(const CreateInfo & createInfo): DescriptorPool()
	{
		createDescriptorPool(createInfo);
	}

	DescriptorPool::DescriptorPool(const CreateInfo2 & createInfo) : DescriptorPool()
	{
		createDescriptorPool(createInfo);
	}

	DescriptorPool::DescriptorPool(const std::vector<VkDescriptorPoolSize> & poolSizes, uint32_t maxSets, VkDescriptorPoolCreateFlags flags): DescriptorPool()
	{
		createDescriptorPool(poolSizes, maxSets, flags);
	}

	void DescriptorPool::createDescriptorPool(const CreateInfo2 & createInfo)
	{
		poolSizes_m.reserve(12);

		for (int descrSizeType = VK_DESCRIPTOR_TYPE_BEGIN_RANGE; descrSizeType != VK_DESCRIPTOR_TYPE_END_RANGE; descrSizeType++) {
			poolSizes_m.emplace_back(VkDescriptorPoolSize { static_cast<VkDescriptorType>(descrSizeType), createInfo(descrSizeType) });
		}
		
		poolSizes_m.emplace_back(VkDescriptorPoolSize{ static_cast<VkDescriptorType>(VK_DESCRIPTOR_TYPE_INLINE_UNIFORM_BLOCK_EXT), createInfo(VK_DESCRIPTOR_TYPE_INLINE_UNIFORM_BLOCK_EXT) });
		poolSizes_m.emplace_back(VkDescriptorPoolSize{ static_cast<VkDescriptorType>(VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_NV), createInfo(VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_NV) });

		createDescriptorPool(poolSizes_m, createInfo.maxSets, createInfo.flags);
	}

	void DescriptorPool::createDescriptorPool(const CreateInfo & createInfo)
	{
		createDescriptorPool(createInfo.poolSizes, createInfo.maxSets, createInfo.flags);
	}

	void DescriptorPool::createDescriptorPool(const std::vector<VkDescriptorPoolSize> & poolSizes, uint32_t maxSets, VkDescriptorPoolCreateFlags flags)
	{
		poolSizes_m = poolSizes;
		maxSets_m = maxSets;
		flags_m = flags;

		VkDescriptorPoolCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		createInfo.flags = flags;
		createInfo.maxSets = maxSets;
		createInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
		createInfo.pPoolSizes = poolSizes.data();

		vkw::Debug::errorCodeCheck(vkCreateDescriptorPool(registry.device, &createInfo, nullptr, vkObject), "Failed to create Descriptor Pool");
	}

	void DescriptorPool::resetDescriptorPool(VkDescriptorPoolResetFlags flags)
	{
		vkw::Debug::errorCodeCheck(vkResetDescriptorPool(registry.device, *vkObject, flags), "Failed to reset the command Pool");
	}

	DescriptorPool & DescriptorPool::operator=(const DescriptorPool & p)
	{
		poolSizes_m = p.poolSizes_m;
		maxSets_m = p.maxSets_m;
		flags_m = p.flags_m;

		return *this;
	}





	/// Descriptor Set Layout
	DescriptorSetLayout::DescriptorSetLayout() :
		layoutBindings(layoutBindings_m)
	{}

	DescriptorSetLayout::DescriptorSetLayout(const CreateInfo & createInfo) : DescriptorSetLayout()
	{
		createDescriptorSetLayout(createInfo);
	}

	DescriptorSetLayout::DescriptorSetLayout(const std::vector<VkDescriptorSetLayoutBinding> & bindings, VkDescriptorSetLayoutCreateFlags flags) : DescriptorSetLayout()
	{
		createDescriptorSetLayout(bindings, flags);
	}

	void DescriptorSetLayout::createDescriptorSetLayout(const CreateInfo & createInfo)
	{
		createDescriptorSetLayout(createInfo.layoutBindings, createInfo.flags);
	}

	void DescriptorSetLayout::createDescriptorSetLayout(const std::vector<VkDescriptorSetLayoutBinding> & bindings, VkDescriptorSetLayoutCreateFlags flags)
	{
		for (auto x : bindings) {
			layoutBindings_m[x.binding] = x;
		}

		VkDescriptorSetLayoutCreateInfo layoutInfo = vkw::Init::descriptorSetLayoutCreateInfo();
		layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
		layoutInfo.pBindings = bindings.data();
		layoutInfo.flags = flags;
		vkw::Debug::errorCodeCheck(vkCreateDescriptorSetLayout(registry.device, &layoutInfo, nullptr, vkObject), "Failed to create DescriptorSetLayout");
	}

	DescriptorSetLayout & DescriptorSetLayout::operator=(const DescriptorSetLayout & rhs)
	{
		layoutBindings_m = rhs.layoutBindings;
		flags = rhs.flags;

		return *this;
	}





	/// Descriptor Set
	DescriptorSet::DescriptorSet() :
		layout(layout_m)
	{}

	DescriptorSet::DescriptorSet(const CreateInfo & createInfo) :
		DescriptorSet(createInfo.descriptorPool, createInfo.layout)
	{}

	DescriptorSet::DescriptorSet(VkDescriptorPool descriptorPool, const DescriptorSetLayout & layout): DescriptorSet()
	{
		allocateDescriptorSet(descriptorPool, layout);
	}

	void DescriptorSet::allocateDescriptorSet(const CreateInfo & createInfo)
	{
		allocateDescriptorSet(createInfo.descriptorPool, createInfo.layout);
	}

	void DescriptorSet::allocateDescriptorSet(VkDescriptorPool descriptorPool, const DescriptorSetLayout & layout)
	{
		layout_m = &layout;
		this->descriptorPool = descriptorPool;

		VkDescriptorSetAllocateInfo allocInfo = Init::descriptorSetAllocateInfo();
		allocInfo.descriptorPool = descriptorPool;
		allocInfo.pSetLayouts = layout.get();
		allocInfo.descriptorSetCount = 1;

		Debug::errorCodeCheck(vkAllocateDescriptorSets(registry.device, &allocInfo, vkObject), "Failed to create Descriptor Set");
	}

	DescriptorSet & DescriptorSet::operator=(const DescriptorSet & rhs)
	{
		descriptorPool = rhs.descriptorPool;
		layout_m = rhs.layout_m;

		return *this;
	}

	void DescriptorSet::update(const std::vector<WriteInfo> & writeInfos, const std::vector<CopyInfo> & copyInfos)
	{
		std::vector<VkWriteDescriptorSet> writes;
		
		for (auto x : writeInfos) {
			VkWriteDescriptorSet write = vkw::Init::writeDescriptorSet();
			write.dstSet = *vkObject;
			write.dstBinding = x.dstBinding;
			write.dstArrayElement = x.dstArrayElement;
			write.descriptorCount = x.descriptorCount;
			write.descriptorType = layout->layoutBindings.at(x.dstBinding).descriptorType;
			write.pImageInfo = x.pImageInfo;
			write.pBufferInfo = x.pBufferInfo;
			write.pTexelBufferView = x.pTexelBufferView;
			writes.push_back(write);
		}

		std::vector<VkCopyDescriptorSet> copys;

		for (auto x : copyInfos) {
			VkCopyDescriptorSet copy = Init::copyDescriptorSet();
			copy.descriptorCount = x.descriptorCount;
			copy.dstArrayElement = x.dstArrayElement;
			copy.dstBinding = x.dstBinding;

			copys.push_back(copy);
		}

		vkUpdateDescriptorSets(registry.device, static_cast<uint32_t>(writes.size()), writes.data(), static_cast<uint32_t>(copys.size()), copys.data());
	}





	/// Device Memory
	Memory::Memory() :
		memoryMap(memoryMap_m),
		memoryFlags(memoryFlags_m),
		size(size_m),
		memoryTypeBits(memoryTypeBits_m),
		memoryType(memoryType_m),
		memoryType_m(std::numeric_limits<uint32_t>::max()),
		memoryTypeBits_m(std::numeric_limits<uint32_t>::max())
	{}

	Memory::Memory(const CreateInfo & createInfo) : Memory(createInfo.memoryFlags, createInfo.size)
	{}

	Memory::Memory(VkMemoryPropertyFlags memoryFlags, VkDeviceSize size) :
		Memory()
	{
		memoryFlags_m = memoryFlags;
		size_m = size;
	}

	void Memory::allocateMemory(AllocationInfo & allocInfo)
	{
		memoryFlags_m = allocInfo.memoryFlags;
		allocateMemory(allocInfo.buffers, allocInfo.images, allocInfo.memoryType, allocInfo.additionalSize);
	}

	void Memory::allocateMemory(std::initializer_list<std::reference_wrapper<Buffer>> buffers, std::initializer_list< std::reference_wrapper<Image>> images, uint32_t memoryType, VkDeviceSize additionalSize) 
		// this has to be by reference otherwise memory cannot be set 
		// IDEA: maybe make memory a shared state
		// IDEA: maybe even go as far as giving every Object a customizable internal shared state
	{	
		for (auto & x : buffers) setMemoryTypeBitsBuffer(x);
		for (auto & x : images) setMemoryTypeBitsImage(x);

		VkMemoryAllocateInfo allocInfo = vkw::Init::memoryAllocateInfo();
		allocInfo.allocationSize = size + additionalSize;
		allocInfo.memoryTypeIndex = memoryType == std::numeric_limits<uint32_t>::max() ? findMemoryType() : memoryType;
		Debug::errorCodeCheck(vkAllocateMemory(registry.device, &allocInfo, nullptr, vkObject), "Failed to allocate Memory");

		for (auto & x : buffers) bindBufferToMemory(x);
		for (auto & x : images) bindImageToMemory(x);
	}

	Memory & Memory::operator=(const Memory & rhs)
	{
		memoryMap_m = rhs.memoryMap_m;
		memoryFlags_m = rhs.memoryFlags_m;
		size_m = rhs.size_m;
		memoryTypeBits_m = rhs.memoryTypeBits_m;
		memoryType_m = rhs.memoryType_m;
		memoryRanges = memoryRanges;

		return *this;
	}

	void Memory::setMemoryTypeBitsBuffer(Buffer & buffer)
	{
		VkMemoryRequirements memoryRequirements;

		vkGetBufferMemoryRequirements(registry.device, buffer, &memoryRequirements);
		setMemoryTypeBits(memoryRequirements);

		buffer.sizeInMemory_m = memoryRequirements.size;
		buffer.allignement_m = memoryRequirements.alignment;
	}

	void Memory::setMemoryTypeBitsImage(Image & image)
	{
		VkMemoryRequirements memoryRequirements;
		vkGetImageMemoryRequirements(registry.device, image, &memoryRequirements);
		setMemoryTypeBits(memoryRequirements);
	}

	void Memory::bindBufferToMemory(Buffer & buffer)
	{
		VKW_assert(buffer.memory == nullptr, "Buffer is already bound to memory");

		buffer.memory = this;
		buffer.offset_m = getOffset(buffer.sizeInMemory, size, memoryRanges);

		memoryRanges[buffer.offset] = buffer.sizeInMemory;

		Debug::errorCodeCheck(vkBindBufferMemory(registry.device, buffer, *vkObject, buffer.offset), "Failed to bind Memory to Buffer");
	}

	void Memory::bindImageToMemory(Image & image)
	{
		VKW_assert(image.memory == nullptr, "image is already bound to memory");

		image.memory = this;
		Debug::errorCodeCheck(vkBindImageMemory(registry.device, image, *vkObject, 0), "Failed to bind Memory to Image");
	}

	void * Memory::map(VkDeviceSize size, VkDeviceSize offset, VkMemoryMapFlags flags)
	{
		memoryMap_m.offset = offset;
		memoryMap_m.size = size;
		vkw::Debug::errorCodeCheck(vkMapMemory(registry.device, *vkObject, offset, size, flags, &memoryMap_m.mapped), "Failed to map memory");
		return memoryMap.mapped;
	}

	void Memory::unMap()
	{
		VKW_assert(memoryMap.mapped != nullptr, "Memory is not mapped");

		vkUnmapMemory(registry.device, *vkObject);
		memoryMap_m.mapped = nullptr;
		memoryMap_m.offset = 0;
		memoryMap_m.size = 0;
	}

	uint32_t Memory::findMemoryType()
	{
		VkPhysicalDeviceMemoryProperties memProperties = registry.physicalDevice.memoryProperties;

		for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
			if ((memoryTypeBits & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & memoryFlags) == memoryFlags) {
				memoryType_m = i;
				return i;
			}
		}

		throw std::runtime_error("failed to find suitable memory type!");
	}

	VkDeviceSize Memory::getOffset(VkDeviceSize dataSize, VkDeviceSize maxSize, std::map<VkDeviceSize, VkDeviceSize> & memoryRanges, VkDeviceSize allignement)
	{
		VkDeviceSize offset = 0;
		dataSize = static_cast<VkDeviceSize>(std::ceil(dataSize / allignement) * allignement);

		for (auto x : memoryRanges) {  //x.first = offset, x.second = size
			if (x.first < offset + dataSize) {
				offset = x.first + x.second;
			}
			else {
				break;
			}
		}

		if (offset + dataSize > maxSize) {
			VKW_PRINT("not enough memory Alocated");
			return std::numeric_limits<uint64_t>::max();
		}

		memoryRanges[offset] = dataSize;
		return offset;
	}

	void Memory::setMemoryTypeBits(VkMemoryRequirements & memoryRequirements)
	{
		memoryTypeBits_m = memoryTypeBits_m & memoryRequirements.memoryTypeBits;
		size_m += memoryRequirements.size;
	}





	/// Buffer
	Buffer::Buffer():
		sizeInMemory(sizeInMemory_m),
		allignement(allignement_m),
		offset(offset_m),
		size(size_m)
	{}

	Buffer::Buffer(const CreateInfo & createInfo) : Buffer()
	{
		createBuffer(createInfo);
	}

	Buffer::Buffer(VkBufferUsageFlags usageFlags, VkDeviceSize size, VkSharingMode sharingMode, VkDeviceSize offset, VkBufferCreateFlags createflags) : Buffer()
	{
		createBuffer(usageFlags, size, sharingMode, offset, createflags);
	}

	Buffer::~Buffer()
	{
		if (memory) {
			if (memory->memoryRanges.size() > 0) { // if size > 0 memory has been most likely destroid
				memory->memoryRanges.erase(offset); // NOTE: review if this is avtually the case
			}
		}
	}

	void Buffer::createBuffer(const CreateInfo & createInfo)
	{
		createBuffer(createInfo.usageFlags, createInfo.size, createInfo.sharingMode, createInfo.offset, createInfo.createflags);
	}

	void Buffer::createBuffer(VkBufferUsageFlags usageFlags, VkDeviceSize size, VkSharingMode sharingMode, VkDeviceSize offset, VkBufferCreateFlags createflags)
	{
		this->usageFlags = usageFlags;
		this->flags = createflags;
		this->sharingMode = sharingMode;
		this->size_m = size;
		this->offset_m = offset;

		VkBufferCreateInfo bufferInfo = vkw::Init::bufferCreateInfo();
		bufferInfo.flags = createflags;
		bufferInfo.size = size;
		bufferInfo.sharingMode = sharingMode;
		bufferInfo.usage = usageFlags;
		// NOTE: look at how this works
		// bufferInfo.pQueueFamilyIndices = 
		// bufferInfo.queueFamilyIndexCount = 

		vkw::Debug::errorCodeCheck(vkCreateBuffer(registry.device, &bufferInfo, nullptr, vkObject), "Failed to create buffer");
	}

	Buffer & Buffer::operator=(const Buffer & rhs)
	{
		usageFlags = rhs.usageFlags;
		flags = rhs.flags;
		sharingMode = rhs.sharingMode;

		size_m = rhs.size_m;
		offset_m = rhs.offset_m;
		sizeInMemory_m = rhs.sizeInMemory_m;
		allignement_m = rhs.allignement_m;
		memoryRanges = rhs.memoryRanges;
		memory = memory;

		return *this;
	}

	void Buffer::write(const void * data, size_t sizeOfData, VkDeviceSize offset, bool leaveMapped) // offset is not relative to the bound memory block but to the start of the buffer 
	{
		VKW_assert(((offset + sizeOfData) <= size), "Invalid Parameters when writing to buffer");

		VkDeviceSize offsetInUse = offset + this->offset;

		if (memory->memoryMap.mapped == nullptr) { // if memory is not mapped
			memory->map(sizeOfData, offsetInUse);
			memcpy(memory->memoryMap.mapped, data, static_cast<size_t>(sizeOfData));
		}  
		else if (memory->memoryMap.offset <= offsetInUse && 
				memory->memoryMap.offset + memory->memoryMap.size >= offsetInUse + sizeOfData) {// if memory is mapped and encompassitates the needed area
			memcpy(reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(memory->memoryMap.mapped) - memory->memoryMap.offset + offsetInUse), data, static_cast<size_t>(sizeOfData));
		}
		else { // if memory is mapped and does not encompassitate the needed area
			memory->unMap();
			memory->map(sizeOfData, offsetInUse);
			memcpy(memory->memoryMap.mapped, data, static_cast<size_t>(sizeOfData));
		} 

		if (leaveMapped == false) { memory->unMap(); }	
	}

	void Buffer::copyFromBuffer(VkBuffer srcBuffer, VkBufferCopy copyRegion, VkCommandPool cmdPool)
	{
		Fence fence(0);

		VkCommandPool commandPool = cmdPool ?  cmdPool : registry.transferCommandPool;
		CommandBuffer commandBuffer(commandPool, VK_COMMAND_BUFFER_LEVEL_PRIMARY);
		commandBuffer.beginCommandBuffer(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

		if (copyRegion.size == 0) {
			copyRegion.size = this->size;
		}

		vkCmdCopyBuffer(commandBuffer, srcBuffer, *vkObject, 1, &copyRegion);

		commandBuffer.endCommandBuffer();

		commandBuffer.submitCommandBuffer(registry.transferQueue, {}, fence);
		fence.wait();

		commandBuffer.freeCommandBuffer();
	}

	SubBuffer Buffer::createSubBuffer(VkDeviceSize subBufferSize)
	{
		return SubBuffer(subBufferSize, Memory::getOffset(subBufferSize, size, this->memoryRanges), this);
	}

	//void Buffer::copyFrom(VkImage image, VkBufferCopy copyRegion, VkCommandPool cmdPool)
	//{
	//	Fence fence;
	//
	//	VkCommandPool commandPool = cmdPool ? cmdPool : registry.transferCommandPool;
	//	CommandBuffer commandBuffer(commandPool, VK_COMMAND_BUFFER_LEVEL_PRIMARY);
	//	commandBuffer.beginCommandBuffer(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
	//
	//	//vkCmdCopyImageToBuffer(commandBuffer, image, )
	//
	//	commandBuffer.endCommandBuffer();
	//
	//	commandBuffer.submitCommandBuffer(Info::deviceQueuesInUse->transferQueue, {}, fence);
	//	fence.wait();
	//
	//	commandBuffer.freeCommandBuffer();
	//	fence.deleteObject();
	//}


	


	/// Sub Buffer
	SubBuffer::SubBuffer():
		size(size_m),
		offset(offset_m)
	{}

	SubBuffer::SubBuffer(VkDeviceSize size, VkDeviceSize offset, Buffer * buffer):
		size(size_m),
		offset(offset_m),
		buffer(buffer),
		offset_m(offset),
		size_m(size)
	{}

	SubBuffer::~SubBuffer()
	{
		buffer->memoryRanges.erase(offset);
	}

	SubBuffer & SubBuffer::operator=(const SubBuffer & rhs)
	{
		if (buffer) clear();

		size_m = rhs.size_m;
		offset_m = rhs.offset_m;

		buffer = rhs.buffer;

		return *this;
	}

	void SubBuffer::write(const void * data, size_t sizeOfData, bool leaveMapped)
	{
		buffer->write(data, sizeOfData, this->offset, leaveMapped);
	}

	void SubBuffer::copyFrom(SubBuffer & srcBuffer, VkCommandPool commandPool)
	{
		buffer->copyFromBuffer(*srcBuffer.buffer, VkBufferCopy{srcBuffer.offset, this->offset, srcBuffer.size}, commandPool);
	}

	void SubBuffer::clear()
	{
		buffer->memoryRanges.erase(offset);
	}






	/// Image
	Image::Image():
		layout(layout_m),
		extent(extent_m)
	{}

	Image::Image(const CreateInfo & createInfo): Image()
	{
		createImage(createInfo);
	}

	Image::Image(VkExtent3D extent, VkFormat format, VkImageUsageFlags usage, VkImageType imageType, VkImageCreateFlags flags) : Image()
	{
		createImage(extent, format, usage, imageType, flags);
	}

	void Image::createImage(const CreateInfo & createInfo)
	{
		layout_m = createInfo.layout;
		extent_m = createInfo.extent;

		imageType = createInfo.imageType;
		sharingMode = createInfo.sharingMode;
		samples = createInfo.samples;
		mipLevels = createInfo.mipLevels;
		arrayLayers = createInfo.arrayLayers;
		tiling = createInfo.tiling;
		format = createInfo.format;
		usage = createInfo.usage;
		familyQueueIndicies = createInfo.familyQueueIndicies;

		VkImageCreateInfo info = vkw::Init::imageCreateInfo();
		info.flags = createInfo.flags;
		info.imageType = createInfo.imageType;
		info.format = createInfo.format;
		info.extent = createInfo.extent;
		info.mipLevels = createInfo.mipLevels;
		info.arrayLayers = createInfo.arrayLayers;
		info.samples = createInfo.samples;
		info.tiling = createInfo.tiling;
		info.usage = createInfo.usage;
		info.sharingMode = createInfo.sharingMode;
		info.queueFamilyIndexCount = static_cast<uint32_t>(createInfo.familyQueueIndicies.size());
		info.pQueueFamilyIndices = createInfo.familyQueueIndicies.data();
		info.initialLayout = createInfo.layout;

		vkw::Debug::errorCodeCheck(vkCreateImage(registry.device, &info, nullptr, vkObject), "Failed to create Image");

	}

	void Image::createImage(VkExtent3D extent, VkFormat format, VkImageUsageFlags usage, VkImageType imageType, VkImageCreateFlags flags)
	{
		CreateInfo createInfo;
		createInfo.format = format;
		createInfo.extent = extent;
		createInfo.usage = usage;
		createInfo.imageType = imageType;
		createInfo.flags = flags;

		createImage(createInfo);
	}

	void Image::transitionImageLayout(VkImageLayout newLayout, VkCommandPool cmdPool, VkImageSubresourceRange range, VkAccessFlags srcAccess, VkAccessFlags dstAccess)
	{
		Fence fence;
		VkCommandPool commandPool = cmdPool == VK_NULL_HANDLE ? registry.transferCommandPool : cmdPool;
		CommandBuffer commandBuffer(commandPool);
		commandBuffer.beginCommandBuffer(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

		VkPipelineStageFlags sourceStage;
		VkPipelineStageFlags destinationStage;

		VkImageMemoryBarrier barrier = vkw::Init::imageMemoryBarrier();
		barrier.oldLayout = this->layout;
		barrier.newLayout = this->layout_m = newLayout;
		barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.image = *vkObject;

		if (range.aspectMask & VK_IMAGE_ASPECT_FLAG_BITS_MAX_ENUM) {
			barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;  // subrecourse range describes what the image's purpose is
			barrier.subresourceRange.baseMipLevel = 0;
			barrier.subresourceRange.levelCount = 1;
			barrier.subresourceRange.baseArrayLayer = 0;
			barrier.subresourceRange.layerCount = 1;
			barrier.subresourceRange.layerCount = arrayLayers;
			barrier.subresourceRange.levelCount = mipLevels;
		}
		else {
			barrier.subresourceRange = range;
		}


		if (newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
			barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
		}


		if (barrier.oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
			barrier.srcAccessMask = 0;
			barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

			sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
			destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		}
		else if (barrier.oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
			barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

			sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
			destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
		}
		else if (barrier.oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
			barrier.srcAccessMask = 0;
			barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

			sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
			destinationStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
		}
		else {
			throw std::invalid_argument("unsupported layout transition!");
		}

		vkCmdPipelineBarrier(
			commandBuffer,
			sourceStage, destinationStage,
			0,
			0, nullptr,
			0, nullptr,
			1, &barrier
		);


		commandBuffer.endCommandBuffer();
		commandBuffer.submitCommandBuffer(registry.transferQueue, {}, fence);
		fence.wait();
		vkQueueWaitIdle(registry.transferQueue);

		commandBuffer.freeCommandBuffer();
		fence.destroyObject();
	}

	//void Image::writeWithStagingBuffer(void * data, VkDeviceSize size, VkImageLayout desiredImageLayout, VkCommandPool transferPool) // this should be write(...) and should be implememntet in  class Texture2D : public : Image
	//{
	//	VkCommandPool cmdPool = transferPool == VK_NULL_HANDLE ? this->registry->transferCommandPool : transferPool;
	//
	//	if (this->usage & VK_IMAGE_USAGE_TRANSFER_DST_BIT) {  // otherwise throw error
	//		Vk::Resources::Memory stagingMemory(VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
	//		Vk::Resources::Buffer stagingBuffer(VK_BUFFER_USAGE_TRANSFER_SRC_BIT, 0, size, VK_SHARING_MODE_EXCLUSIVE, 0);
	//
	//		stagingMemory.allocateMemory({ stagingBuffer });
	//		stagingBuffer.write(data, static_cast<size_t>(size), 0, false);
	//
	//		this->transitionImageLayout(VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, cmdPool);
	//
	//		VkImageSubresourceLayers subResource = Vk::Init::imageSubresourceLayers(VK_IMAGE_ASPECT_COLOR_BIT);
	//		VkBufferImageCopy region = Vk::Init::bufferImageCopy({ extent.width, extent.height, 1 }, subResource, { 0,0,0 }, 0);
	//
	//		this->copyFromBuffer(stagingBuffer, { region }, cmdPool);
	//
	//		if (desiredImageLayout != VK_IMAGE_LAYOUT_MAX_ENUM) { // otherwise do nothing
	//			this->transitionImageLayout(desiredImageLayout, cmdPool);
	//		}
	//	}
	//}

	void Image::copyFromImage(Image & srcImage, std::vector<VkImageCopy> regions, VkCommandPool cmdPool)
	{
		if (regions.size() == 0) {
			VkImageSubresourceLayers subResource = {};
			subResource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			subResource.mipLevel = 0;
			subResource.layerCount = 1;
			subResource.baseArrayLayer = 0;

			VkImageCopy region = {};
			region.extent = extent;
			region.srcOffset = { 0,0,0 };
			region.srcSubresource = subResource;
			region.dstOffset = { 0,0,0 };
			region.dstSubresource = subResource;
			regions.push_back(region);
		}
		vkw::Fence fence(0);
		VkCommandPool commandPool = (cmdPool == VK_NULL_HANDLE) ? registry.transferCommandPool : cmdPool;
		vkw::CommandBuffer commandBuffer(commandPool);
		commandBuffer.beginCommandBuffer(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

		vkCmdCopyImage(commandBuffer, srcImage, srcImage.layout, *vkObject, layout, static_cast<uint32_t>(regions.size()), regions.data());

		commandBuffer.endCommandBuffer();
		commandBuffer.submitCommandBuffer(registry.transferQueue, {}, fence);
		fence.wait();

		commandBuffer.freeCommandBuffer();
		fence.destroyObject();
	}

	void Image::copyFromBuffer(Buffer & srcBuffer, std::vector<VkBufferImageCopy> copyRegions, VkCommandPool cmdPool) // TODO: switch param copyRegions with param srcBuffer
	{
		if (copyRegions.size() == 0) {
			VkImageSubresourceLayers subResource = {};
			subResource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			subResource.mipLevel = 0;
			subResource.layerCount = 1;
			subResource.baseArrayLayer = 0;

			VkBufferImageCopy region = {};
			region.bufferOffset = 0;
			region.bufferImageHeight = 0;
			region.bufferRowLength = 0;
			region.imageSubresource = subResource;
			region.imageOffset = { 0,0,0 };
			region.imageExtent = { extent.width, extent.height, 1 };

			copyRegions.push_back(region);
		}

		vkw::Fence fence(0);
		VkCommandPool commandPool = cmdPool == VK_NULL_HANDLE ? registry.transferCommandPool : cmdPool;
		vkw::CommandBuffer commandBuffer(commandPool);
		commandBuffer.beginCommandBuffer(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

		vkCmdCopyBufferToImage(commandBuffer, srcBuffer, *vkObject, layout, static_cast<uint32_t>(copyRegions.size()), copyRegions.data());

		commandBuffer.endCommandBuffer();
		commandBuffer.submitCommandBuffer(registry.transferQueue, {}, fence);
		fence.wait();

		commandBuffer.freeCommandBuffer();
		fence.destroyObject();
	}
	




	/// Image View
	ImageView::ImageView(const CreateInfo & createInfo)
	{
		createImageView(createInfo);
	}

	ImageView::ImageView(const Image & image, VkImageSubresourceRange subresource, VkImageViewType viewType, VkComponentMapping components)
	{
		createImageView(image, subresource, viewType, components);
	}

	void ImageView::createImageView(const CreateInfo & createInfo)
	{
		createImageView(createInfo.image, createInfo.subresource, createInfo.viewType, createInfo.components);
	}

	void ImageView::createImageView(const Image & image, VkImageSubresourceRange subresource, VkImageViewType viewType, VkComponentMapping components)
	{
		VkImageViewCreateInfo createInfo = vkw::Init::imageViewCreateInfo();
		createInfo.image = image;
		createInfo.format = image.format;
		createInfo.viewType = viewType;
		createInfo.subresourceRange = subresource;
		createInfo.components = components;

		vkw::Debug::errorCodeCheck(vkCreateImageView(registry.device, &createInfo, nullptr, vkObject), "Failed to create Image");
	}





	/// Sampler
	Sampler::Sampler(const CreateInfo & createInfo)
	{
		createSampler(createInfo);
	}

	void Sampler::createSampler()
	{
		createSampler(CreateInfo());
	}

	void Sampler::createSampler(const CreateInfo & createInfo)
	{
		filter = createInfo.filter;
		addressMode = createInfo.addressMode;
		mipMap = createInfo.mipMap;
		borderColor = createInfo.borderColor;
		anisotropyEnable = createInfo.anisotropyEnable;
		maxAnisotropy = createInfo.maxAnisotropy;
		unnormalizedCoordinates = createInfo.unnormalizedCoordinates;
		compareEnable = createInfo.compareEnable;
		compareOp = createInfo.compareOp;

		VkSamplerCreateInfo samplerInfo = vkw::Init::samplerCreateInfo();
		samplerInfo.addressModeU = createInfo.addressMode.U;
		samplerInfo.addressModeV = createInfo.addressMode.V;
		samplerInfo.addressModeW = createInfo.addressMode.W;
		samplerInfo.magFilter = createInfo.filter.magFilter;
		samplerInfo.minFilter = createInfo.filter.minFilter;
		samplerInfo.anisotropyEnable = createInfo.anisotropyEnable;
		samplerInfo.maxAnisotropy = createInfo.maxAnisotropy;
		samplerInfo.borderColor = createInfo.borderColor;
		samplerInfo.unnormalizedCoordinates = createInfo.unnormalizedCoordinates;
		samplerInfo.compareEnable = createInfo.compareEnable;
		samplerInfo.compareOp = createInfo.compareOp;
		samplerInfo.mipmapMode = createInfo.mipMap.mipmapMode;
		samplerInfo.mipLodBias = createInfo.mipMap.mipLodBias;
		samplerInfo.minLod = createInfo.mipMap.minLod;
		samplerInfo.maxLod = createInfo.mipMap.maxLod;

		vkw::Debug::errorCodeCheck(vkCreateSampler(registry.device, &samplerInfo, nullptr, vkObject), "Failed to create Sampler");
	}




	/// Frame Buffer
	FrameBuffer::FrameBuffer(const CreateInfo & createInfo)
	{
		createFrameBuffer(createInfo);
	}

	FrameBuffer::FrameBuffer(VkRenderPass renderPass, VkExtent2D extent, std::vector<VkImageView> attachments, uint32_t layers, VkFramebufferCreateFlags flags)
	{
		createFrameBuffer(renderPass, extent, attachments, layers, flags);
	}

	void FrameBuffer::createFrameBuffer(const CreateInfo & createInfo)
	{
		createFrameBuffer(createInfo.renderPass, createInfo.extent, createInfo.attachments, createInfo.layers, createInfo.flags);
	}

	void FrameBuffer::createFrameBuffer(VkRenderPass renderPass, VkExtent2D extent, std::vector<VkImageView> attachments, uint32_t layers, VkFramebufferCreateFlags flags)
	{
		this->flags = flags;
		this->renderPass = renderPass;
		this->attachments = attachments;
		this->extent = extent;
		this->layers = layers;

		VkFramebufferCreateInfo createInfo = Init::framebufferCreateInfo();
		createInfo.flags = flags;
		createInfo.renderPass = renderPass;
		createInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
		createInfo.pAttachments = attachments.data();
		createInfo.width = extent.width;
		createInfo.height = extent.height;
		createInfo.layers = layers;

		vkw::Debug::errorCodeCheck(vkCreateFramebuffer(registry.device, &createInfo, nullptr, vkObject), "Failed to create FrameBuffer");
	}

}