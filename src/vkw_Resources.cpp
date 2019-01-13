#include "vkw_Resources.h"

#include "vkw_Operations.h"
#include "vkw_Assets.h"
#include <math.h>


namespace vkw {
	DescriptorPool::DescriptorPool():
		poolSizes(poolSizes_m),
		maxSets(maxSets_m),
		flags(flags_m)
	{
	}
	/// Descriptor Pool
	DescriptorPool::DescriptorPool(const CreateInfo & createInfo): DescriptorPool()
	{
		createDescriptorPool(createInfo);
	}

	DescriptorPool::DescriptorPool(const CreateInfo2 & createInfo) : DescriptorPool()
	{
		createDescriptorPool(createInfo);
	}

	DescriptorPool::DescriptorPool(const std::vector<VkDescriptorPoolSize> & poolSizes, uint32_t maxSets, VkDescriptorPoolCreateFlags flags):
		poolSizes(poolSizes), 
		maxSets(maxSets), 
		flags(flags)
	{
		createDescriptorPool(poolSizes, maxSets, flags);
	}

	DescriptorPool & DescriptorPool::operator=(const DescriptorPool & p)
	{
		poolSizes_m = p.poolSizes_m;
		maxSets_m = p.maxSets_m;
		flags_m = p.flags_m;

		return *this;
	}

	/* DescriptorPool & DescriptorPool::operator=(DescriptorPool && p)
	{
		poolSizes_m = p.poolSizes_m;
		maxSets_m = p.maxSets_m;
		flags_m = p.flags_m;

		return *this;
	}*/

	void DescriptorPool::createDescriptorPool(const CreateInfo & createInfo)
	{
		createDescriptorPool(createInfo.poolSizes, createInfo.maxSets, createInfo.flags);
	}

	void DescriptorPool::createDescriptorPool(const CreateInfo2 & createInfo)
	{
		//createDescriptorPool(createInfo);
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





	/// Descriptor Set Layout
	DescriptorSetLayout::DescriptorSetLayout(std::vector<VkDescriptorSetLayoutBinding> bindings, VkDescriptorSetLayoutCreateFlags flags):
		flags(flags)
	{
		for (auto x : bindings) {
			layoutBindings[x.binding] = x;
		}
		createDescriptorSetLayout();
	}


	void DescriptorSetLayout::createDescriptorSetLayout()
	{
		
		std::vector<VkDescriptorSetLayoutBinding> bindings;
		for (auto x : layoutBindings) {
			bindings.push_back(x.second);
		}

		VkDescriptorSetLayoutCreateInfo layoutInfo = vkw::Init::descriptorSetLayoutCreateInfo();
		layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
		layoutInfo.pBindings = bindings.data();
		layoutInfo.flags = flags;
		vkw::Debug::errorCodeCheck(vkCreateDescriptorSetLayout(registry.device, &layoutInfo, nullptr, vkObject), "Failed to create DescriptorSetLayout");
	}






	/// Descriptor Set
	DescriptorSet::DescriptorSet(VkDescriptorPool descriptorPool, DescriptorSetLayout & layout, bool shouldAllocateDescriptorSet):
		descriptorPool(descriptorPool), layout(&layout)
	{
		allocateDescriptorSet();
	}



	void DescriptorSet::allocateDescriptorSet()
	{
		

		VkDescriptorSetAllocateInfo allocInfo = Init::descriptorSetAllocateInfo();
		allocInfo.descriptorPool = descriptorPool;
		allocInfo.pSetLayouts = layout->get();
		allocInfo.descriptorSetCount = 1;

		vkw::Debug::errorCodeCheck(vkAllocateDescriptorSets(registry.device, &allocInfo, vkObject), "Failed to create Descriptor Set");
	}


	void DescriptorSet::update(std::vector<WriteInfo> * writeInfos, std::vector<CopyInfo> * copyInfos)
	{
		std::vector<VkWriteDescriptorSet> writes;
		
		for (auto x : *writeInfos) {
			VkWriteDescriptorSet write = vkw::Init::writeDescriptorSet();
			write.dstSet = *vkObject;
			write.dstBinding = x.dstBinding;
			write.dstArrayElement = x.dstArrayElement;
			write.descriptorCount = x.descriptorCount;
			write.descriptorType = this->layout->layoutBindings[x.dstBinding].descriptorType;
			write.pImageInfo = x.pImageInfo;
			write.pBufferInfo = x.pBufferInfo;
			write.pTexelBufferView = x.pTexelBufferView;
			writes.push_back(write);
		}


		std::vector<VkCopyDescriptorSet> copys;

		for (auto x : *copyInfos) {
			VkCopyDescriptorSet copy = Init::copyDescriptorSet();
			copy.descriptorCount = x.descriptorCount;
			copy.dstArrayElement = x.dstArrayElement;
			copy.dstBinding = x.dstBinding;

			copys.push_back(copy);
		}


		vkUpdateDescriptorSets(registry.device, static_cast<uint32_t>(writes.size()), writes.data(), static_cast<uint32_t>(copys.size()), copys.data());
	}








	/// Device Memory
	Memory::Memory(VkMemoryPropertyFlags memoryFlags, VkDeviceSize size) :
		memoryFlags(memoryFlags),
		size(size)
	{}


	void Memory::allocateMemory(std::initializer_list<std::reference_wrapper<Buffer>> buffers, std::initializer_list<std::reference_wrapper<Image>> images) // this has to be by reference
	{	
		for (auto x : buffers) setMemoryTypeBitsBuffer(x);
		for (auto x : images) setMemoryTypeBitsImage(x);

		VkMemoryAllocateInfo allocInfo = vkw::Init::memoryAllocateInfo();
		allocInfo.allocationSize = size;
		allocInfo.memoryTypeIndex = findMemoryType();
		Debug::errorCodeCheck(vkAllocateMemory(registry.device, &allocInfo, nullptr, vkObject), "Failed to allocate Memory");

		for (auto x : buffers) bindBufferToMemory(x);
		for (auto x : images) bindImageToMemory(x);
	}


	void Memory::setMemoryTypeBitsBuffer(Buffer & buffer)
	{
		VkMemoryRequirements memoryRequirements;

		vkGetBufferMemoryRequirements(registry.device, buffer, &memoryRequirements);
		setMemoryTypeBits(memoryRequirements);

		buffer.sizeInMemory = memoryRequirements.size;
		buffer.allignement = memoryRequirements.alignment;
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
		buffer.offset = getOffset(buffer.sizeInMemory, size, memoryRanges);

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
		memoryMap.offset = offset;
		memoryMap.size = size;
		vkw::Debug::errorCodeCheck(vkMapMemory(registry.device, *vkObject, offset, size, flags, &memoryMap.mapped), "Failed to map memory");
		return memoryMap.mapped;
	}


	void Memory::unMap()
	{
		VKW_assert(memoryMap.mapped != nullptr, "Memory is not mapped");

		vkUnmapMemory(registry.device, *vkObject);
		memoryMap.mapped = nullptr;
		memoryMap.offset = 0;
		memoryMap.size = 0;
	}


	uint32_t Memory::findMemoryType()
	{
		VkPhysicalDeviceMemoryProperties memProperties = registry.physicalDevice.memoryProperties;

		for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
			if ((memoryTypeBits & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & memoryFlags) == memoryFlags) {
				memoryType = i;
				return i;
			}
		}

		throw std::runtime_error("failed to find suitable memory type!");
	}


	VkDeviceSize Memory::getOffset(VkDeviceSize & dataSize, VkDeviceSize maxSize, std::map<VkDeviceSize, VkDeviceSize> & memoryRanges, VkDeviceSize allignement)
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
		memoryTypeBits = memoryTypeBits & memoryRequirements.memoryTypeBits;
		size += memoryRequirements.size;
	}








	/// Buffer
	Buffer::Buffer(CreateInfo createInfo) :
		usageFlags(createInfo.usageFlags),
		createFlags(createInfo.createflags),
		size(createInfo.size),
		offset(createInfo.offset),
		sharingMode(createInfo.sharingMode)
	{
		createBuffer();
	}


	Buffer::Buffer(VkBufferUsageFlags usageFlags, VkDeviceSize size, VkSharingMode sharingMode, VkDeviceSize offset, VkBufferCreateFlags createflags) :
		usageFlags(usageFlags),
		createFlags(createflags),
		size(size),
		offset(offset),
		sharingMode(sharingMode)
	{
		createBuffer();
	}


	Buffer::~Buffer()
	{
		if (memory) {
			if (memory->memoryRanges.size() > 0) { // if size > 0 memory has been most likely destroid
				memory->memoryRanges.erase(offset);
			}
		}
	}


	void Buffer::createBuffer()
	{
		

		VkBufferCreateInfo bufferInfo = vkw::Init::bufferCreateInfo();
		bufferInfo.flags = createFlags;
		bufferInfo.size = size;
		bufferInfo.sharingMode = sharingMode;
		bufferInfo.usage = usageFlags;

		vkw::Debug::errorCodeCheck(vkCreateBuffer(registry.device, &bufferInfo, nullptr, vkObject), "Failed to create buffer");
	}


	void Buffer::write(const void * data, size_t sizeOfData, VkDeviceSize offset, bool leaveMapped) // offset is not relative to the bound memory block but to the start of the buffer 
	{
		VKW_assert(((offset + sizeOfData) <= size), "Invalid Parameters when writing to buffer");

		VkDeviceSize offsetInUse = offset + this->offset;

		if (memory->memoryMap.mapped == nullptr) { // if memory is not mapped
			memory->map(sizeOfData, offsetInUse);
			memcpy(memory->memoryMap.mapped, data, static_cast<size_t>(sizeOfData));
		}  
		else if (memory->memoryMap.offset <= offsetInUse && memory->memoryMap.offset + memory->memoryMap.size >= offsetInUse + sizeOfData) {// if memory is mapped and encompassitates the needed area
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
		Fence fence;

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
		return SubBuffer(subBufferSize, Memory::getOffset(subBufferSize, size, this->memoryRanges), *this);
	}

	//void Buffer::copyFrom(VkImage image, VkBufferCopy copyRegion, VkCommandPool cmdPool)
	//{
	//	Fence fence;

	//	VkCommandPool commandPool = cmdPool ? cmdPool : registry.transferCommandPool;
	//	CommandBuffer commandBuffer(commandPool, VK_COMMAND_BUFFER_LEVEL_PRIMARY);
	//	commandBuffer.beginCommandBuffer(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

	//	//vkCmdCopyImageToBuffer(commandBuffer, image, )

	//	commandBuffer.endCommandBuffer();

	//	commandBuffer.submitCommandBuffer(Info::deviceQueuesInUse->transferQueue, {}, fence);
	//	fence.wait();

	//	commandBuffer.freeCommandBuffer();
	//	fence.deleteObject();
	//}


	
	


	/// Sub Buffer
	SubBuffer::SubBuffer(VkDeviceSize size, VkDeviceSize offset, Buffer & buffer):
		buffer(buffer),
		size(size),
		offset(offset)
	{}


	SubBuffer::~SubBuffer()
	{
		buffer.memoryRanges.erase(offset);
	}


	void SubBuffer::write(const void * data, size_t sizeOfData, bool leaveMapped)
	{
		buffer.write(data, sizeOfData, this->offset, leaveMapped);
	}


	void SubBuffer::copyFrom(SubBuffer & srcBuffer, VkCommandPool commandPool)
	{
		buffer.copyFromBuffer(srcBuffer.buffer, VkBufferCopy{srcBuffer.offset, this->offset, srcBuffer.size}, commandPool);
	}


	void SubBuffer::clear()
	{
		buffer.memoryRanges.erase(offset);
	}





	/// Image
	Image::Image(VkExtent3D extent, VkFormat format, VkImageUsageFlags usage, uint32_t miplvls, uint32_t layers, VkImageCreateFlags flags) :
		extent(extent),
		format(format),
		usage(usage),
		mipLevels(miplvls),
		arrayLayers(layers),
		flags(flags)
	{
		if (this->extent.depth == 0) {
			this->extent.depth = 1;
		}
		createImage();
	}


	void Image::createImage()
	{
		

		VkImageCreateInfo createInfo = vkw::Init::imageCreateInfo();
		createInfo.flags = flags;
		createInfo.imageType = imageType;
		createInfo.format = format;
		createInfo.extent = extent;
		createInfo.mipLevels = mipLevels;
		createInfo.arrayLayers = arrayLayers;
		createInfo.samples = samples;
		createInfo.tiling = tiling;
		createInfo.usage = usage;
		createInfo.sharingMode = sharingMode;
		createInfo.queueFamilyIndexCount = static_cast<uint32_t>(familyQueueIndicies.size());
		createInfo.pQueueFamilyIndices = familyQueueIndicies.data();
		createInfo.initialLayout = layout;

		vkw::Debug::errorCodeCheck(vkCreateImage(registry.device, &createInfo, nullptr, vkObject), "Failed to create Image");
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
		barrier.newLayout = this->layout = newLayout;
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
		vkw::Fence fence;
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

		vkw::Fence fence;
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
	ImageView::ImageView(Image & image, VkImageSubresourceRange subresource, VkImageViewType viewType, VkComponentMapping components) :
		image(image), subresource(subresource), viewType(viewType), components(components)
	{
		createImageView();
	}


	void ImageView::createImageView()
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
	Sampler::Sampler()
	{
		//createSampler();
	}


	void Sampler::createSampler()
	{
		

		VkSamplerCreateInfo samplerInfo = vkw::Init::samplerCreateInfo();
		samplerInfo.addressModeU = addressMode.addressModeU;
		samplerInfo.addressModeV = addressMode.addressModeV;
		samplerInfo.addressModeW = addressMode.addressModeW;
		samplerInfo.magFilter = filter.magFilter;
		samplerInfo.minFilter = filter.minFilter;
		samplerInfo.anisotropyEnable = anisotropyEnable;
		samplerInfo.maxAnisotropy = maxAnisotropy;
		samplerInfo.borderColor = borderColor;
		samplerInfo.unnormalizedCoordinates = unnormalizedCoordinates;
		samplerInfo.compareEnable = compareEnable;
		samplerInfo.compareOp = compareOp;
		samplerInfo.mipmapMode = mipMap.mipmapMode;
		samplerInfo.mipLodBias = mipMap.mipLodBias;
		samplerInfo.minLod = mipMap.minLod;
		samplerInfo.maxLod = mipMap.maxLod;

		vkw::Debug::errorCodeCheck(vkCreateSampler(registry.device, &samplerInfo, nullptr, vkObject), "Failed to create Sampler");
	}





	/// Frame Buffer
	void FrameBuffer::createFrameBuffer()
	{
		

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