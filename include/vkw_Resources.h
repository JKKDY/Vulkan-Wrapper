#pragma once
#include "vkw_Include.h"



namespace vkw {
	class Buffer;
	class Image;
	class SubBuffer;
    


	/// Descriptor Pool
	class DescriptorPool : public impl::Object<impl::VkwDescriptorPool>{
	public:
		struct CreateInfo : impl::CreateInfo {
			std::vector<VkDescriptorPoolSize> poolSizes;
			uint32_t maxSets;
			VkDescriptorPoolCreateFlags flags = 0;
		};

		struct CreateInfo2 : impl::CreateInfo { 
			uint32_t samplerCount = 0;
			uint32_t combinedImageSamplerCount = 0;
			uint32_t sampledImageCount = 0;
			uint32_t storageImageCount = 0;
			uint32_t uniformTexelBufferCount = 0;
			uint32_t storageTexelBufferCount = 0;
			uint32_t uniformBufferCount = 0;
			uint32_t storageBufferCount = 0;
			uint32_t uniformBufferDynamicCount = 0;
			uint32_t storageBufferDynamicCount = 0;
			uint32_t inputAttachementCount = 0;
			uint32_t inlineUniformBlockEXTCount = 0;
			uint32_t accelerationStructureNVCount = 0;
			uint32_t maxSets;
			VkDescriptorPoolCreateFlags flags = 0;

			uint32_t operator ()(int descr) const;
		};

		VULKAN_WRAPPER_API DescriptorPool();
		VULKAN_WRAPPER_API DescriptorPool(const CreateInfo & createInfo);
		VULKAN_WRAPPER_API DescriptorPool(const CreateInfo2 & createInfo);
		VULKAN_WRAPPER_API DescriptorPool(const std::vector<VkDescriptorPoolSize> & poolSizes, uint32_t maxSets, VkDescriptorPoolCreateFlags flags = 0);

		VULKAN_WRAPPER_API void createDescriptorPool(const CreateInfo & createInfo);
		VULKAN_WRAPPER_API void createDescriptorPool(const CreateInfo2 & createInfo);
		VULKAN_WRAPPER_API void createDescriptorPool(const std::vector<VkDescriptorPoolSize> & poolSizes, uint32_t maxSets, VkDescriptorPoolCreateFlags flags = 0);

		VULKAN_WRAPPER_API DescriptorPool & operator = (const DescriptorPool & rhs);

		const std::vector<VkDescriptorPoolSize> & poolSizes;
		const uint32_t & maxSets;
		const VkDescriptorPoolCreateFlags & flags;

		VULKAN_WRAPPER_API void resetDescriptorPool(VkDescriptorPoolResetFlags flags = 0);
	private:
		std::vector<VkDescriptorPoolSize> poolSizes_m;
		uint32_t maxSets_m;
		VkDescriptorPoolCreateFlags flags_m = 0;
	};





	/// Descriptor Set Layout
	class DescriptorSetLayout : public impl::Object<impl::VkwDescriptorSetLayout> {
	public:
		struct CreateInfo : impl::CreateInfo {
			std::vector<VkDescriptorSetLayoutBinding> layoutBindings;
			VkDescriptorSetLayoutCreateFlags flags = 0;
		};

		VULKAN_WRAPPER_API DescriptorSetLayout();
		VULKAN_WRAPPER_API DescriptorSetLayout(const CreateInfo & createInfo);
		VULKAN_WRAPPER_API DescriptorSetLayout(const std::vector<VkDescriptorSetLayoutBinding> & bindings, VkDescriptorSetLayoutCreateFlags flags = 0);
		VULKAN_WRAPPER_API ~DescriptorSetLayout() = default;

		VULKAN_WRAPPER_API void createDescriptorSetLayout(const CreateInfo & createInfo);
		VULKAN_WRAPPER_API void createDescriptorSetLayout(const std::vector<VkDescriptorSetLayoutBinding> & bindings, VkDescriptorSetLayoutCreateFlags flags = 0);

		VULKAN_WRAPPER_API DescriptorSetLayout & operator = (const DescriptorSetLayout & p);

		const std::map<uint32_t, VkDescriptorSetLayoutBinding> & layoutBindings;
		VkDescriptorSetLayoutCreateFlags flags = 0;
	private:
		std::map<uint32_t, VkDescriptorSetLayoutBinding> layoutBindings_m;
	};






	/// Descriptor Set
	class DescriptorSet : public impl::Object<impl::VkwDescriptorSet> {
	public:
		struct WriteInfo {
			uint32_t dstBinding;
			uint32_t descriptorCount;
			uint32_t dstArrayElement = 0;
			const VkDescriptorImageInfo * pImageInfo = nullptr;
			const VkDescriptorBufferInfo * pBufferInfo = nullptr;
			const VkBufferView * pTexelBufferView = nullptr;
		};

		struct CopyInfo {
			VkDescriptorSet    srcSet;
			uint32_t           srcBinding;
			uint32_t           srcArrayElement;
			uint32_t           dstBinding;
			uint32_t           dstArrayElement;
			uint32_t           descriptorCount;
		};

		struct UpdateInfo {
			std::reference_wrapper<DescriptorSet> descriptor;
			std::vector<WriteInfo> writeInfos;
			std::vector<CopyInfo> copyInfos;
		};

		struct AllocInfo : impl::CreateInfo {
			VkDescriptorPool descriptorPool;
			tools::PtrContainer<DescriptorSetLayout> layout;
		};

		VULKAN_WRAPPER_API DescriptorSet();
		VULKAN_WRAPPER_API DescriptorSet(const AllocInfo & createInfo);
		VULKAN_WRAPPER_API DescriptorSet(VkDescriptorPool descriptorPool, const DescriptorSetLayout & layout);
		VULKAN_WRAPPER_API ~DescriptorSet() = default;

		VULKAN_WRAPPER_API void allocateDescriptorSet(const AllocInfo & createInfo);
		VULKAN_WRAPPER_API void allocateDescriptorSet(VkDescriptorPool descriptorPool, const DescriptorSetLayout & layout);

		VULKAN_WRAPPER_API DescriptorSet & operator = (const DescriptorSet & rhs);

		VkDescriptorPool descriptorPool;
		const DescriptorSetLayout *& layout;

		VULKAN_WRAPPER_API void update(const std::vector<WriteInfo> & writeInfos, const std::vector<CopyInfo> & copyInfos); //TODO implement copying
		//VULKAN_WRAPPER_API void write(uint32_t dstBinding, uint32_t dstArrayElement, uint32_t descriptorCount, const VkDescriptorImageInfo * pImageInfo = nullptr, const VkDescriptorBufferInfo * pBufferInfo = nullptr, const VkBufferView * pTexelBufferView = nullptr);
		
		VULKAN_WRAPPER_API static void allocateDescriptorSets(std::vector<DescriptorSet> descriptorSets);	//TODO: impelement
		VULKAN_WRAPPER_API static void updateeDescriptorSets(std::vector<UpdateInfo> updateInfos);	//TODO: impelement
	private:
		const DescriptorSetLayout * layout_m = nullptr;
	};






	/// Memory
	struct MemoryRanges {
		VULKAN_WRAPPER_API bool query(VkDeviceSize size) const;
		VULKAN_WRAPPER_API void addMoreSize(VkDeviceSize size);
		VULKAN_WRAPPER_API VkDeviceSize add(VkDeviceSize size);	// returns a offset, if failed max<VkDeviceSize>
		VULKAN_WRAPPER_API void add(VkDeviceSize offset, VkDeviceSize size);
		VULKAN_WRAPPER_API void remove(VkDeviceSize offset, VkDeviceSize size);
		VULKAN_WRAPPER_API void reset();
	private:
		std::map<VkDeviceSize, std::multiset<VkDeviceSize>> memoryRanges; // first: offset, second: sizes
		std::map<VkDeviceSize, VkDeviceSize> freeRanges; // first: offset, second: sizes
	};

	class Memory : public impl::Object<impl::VkwDeviceMemory> {
		struct Mapped {
			VkDeviceSize size = 0;
			VkDeviceSize offset = 0;
			void * mapped = nullptr; 
		};
	public:
		struct CreateInfo : impl::CreateInfo {
			VkMemoryPropertyFlags memoryFlags;
			VkDeviceSize size = 0;
		};

		struct AllocInfo : impl::CreateInfo {
			VkMemoryPropertyFlags memoryFlags;
			std::vector<std::reference_wrapper<Buffer>> buffers;
			std::vector<std::reference_wrapper<Image>> images;
			VkDeviceSize additionalSize = 0;
			uint32_t memoryType = std::numeric_limits<uint32_t>::max();
		};

		VULKAN_WRAPPER_API Memory();
		VULKAN_WRAPPER_API Memory(const CreateInfo & createInfo);
		VULKAN_WRAPPER_API Memory(AllocInfo & allocInfo);
		VULKAN_WRAPPER_API Memory(VkMemoryPropertyFlags memoryFlags, VkDeviceSize size = 0);
		VULKAN_WRAPPER_API ~Memory() = default;

		VULKAN_WRAPPER_API void allocateMemory(AllocInfo & allocInfo);
		VULKAN_WRAPPER_API void allocateMemory(std::vector<std::reference_wrapper<Buffer>> buffers = {}, std::vector<std::reference_wrapper<Image>> images = {},  VkDeviceSize additionalSize = 0);
		VULKAN_WRAPPER_API void allocateMemory(VkMemoryPropertyFlags memoryFlags, std::vector<std::reference_wrapper<Buffer>> buffers = {}, std::vector<std::reference_wrapper<Image>> images = {}, VkDeviceSize additionalSize = 0, uint32_t memoryType = std::numeric_limits<uint32_t>::max());

		VULKAN_WRAPPER_API Memory & operator = (const Memory & rhs);

		const VkMemoryPropertyFlags & memoryFlags;
		const Mapped & memoryMap;
		const VkDeviceSize & size;
		const uint32_t & memoryTypeBits;
		const uint32_t & memoryType;
		const MemoryRanges & memoryRanges;

		VULKAN_WRAPPER_API void setMemoryTypeBitsBuffer(Buffer & buffer);
		VULKAN_WRAPPER_API void bindBufferToMemory(Buffer & buffer);

		VULKAN_WRAPPER_API void setMemoryTypeBitsImage(Image & image);
		VULKAN_WRAPPER_API void bindImageToMemory(Image & image);

		VULKAN_WRAPPER_API void setFlags(VkMemoryPropertyFlags memoryFlags);
		VULKAN_WRAPPER_API void setMemoryTypeBits(VkMemoryRequirements & memoryRequirements);
		VULKAN_WRAPPER_API void * map(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0, VkMemoryMapFlags flags = 0);
		VULKAN_WRAPPER_API void unMap();
		VULKAN_WRAPPER_API void flush();
		VULKAN_WRAPPER_API void invalidate();
	private:
		VkDeviceSize size_m = 0;
		Mapped memoryMap_m;
		VkMemoryPropertyFlags memoryFlags_m;
		uint32_t memoryTypeBits_m = std::numeric_limits<uint32_t>::max();
		uint32_t memoryType_m = std::numeric_limits<uint32_t>::max();
		MemoryRanges memoryRanges_m;

		friend Buffer;
	};
	




	/// Buffer
	class Buffer : public impl::Object<impl::VkwBuffer> {
	public:
		struct CreateInfo : impl::CreateInfo {
			VkBufferUsageFlags usageFlags;
			VkBufferCreateFlags createflags;
			VkDeviceSize size; 
			VkSharingMode sharingMode = VK_SHARING_MODE_EXCLUSIVE;
			VkDeviceSize offset = 0; // NOTE: rn not doing anything
		};

		VULKAN_WRAPPER_API Buffer();
		VULKAN_WRAPPER_API Buffer(const CreateInfo & createInfo);
		VULKAN_WRAPPER_API Buffer(VkBufferUsageFlags usageFlags, VkDeviceSize size, VkSharingMode sharingMode = VK_SHARING_MODE_EXCLUSIVE, VkDeviceSize offset = 0, VkBufferCreateFlags createflags = 0);
		VULKAN_WRAPPER_API ~Buffer();

		VULKAN_WRAPPER_API void createBuffer(const CreateInfo & createInfo);
		VULKAN_WRAPPER_API void createBuffer(VkBufferUsageFlags usageFlags, VkDeviceSize size, VkSharingMode sharingMode = VK_SHARING_MODE_EXCLUSIVE, VkDeviceSize offset = 0, VkBufferCreateFlags createflags = 0);

		VULKAN_WRAPPER_API Buffer & operator = (const Buffer & rhs);

		VULKAN_WRAPPER_API void destroyObject() override;

		VkBufferUsageFlags usageFlags;
		VkBufferCreateFlags flags;
		VkSharingMode sharingMode; 
		const VkDeviceSize & size;
		const VkDeviceSize & offset; // offset in Memory
		const VkDeviceSize & sizeInMemory; // Size is alligned with allignement e.g. "size" = usable size, "sizeInMemory" = size the buffer takes up in its Vk::Memory
		const VkDeviceSize & allignement;
		const uint32_t & memoryTypeBits;

		VULKAN_WRAPPER_API SubBuffer createSubBuffer(VkDeviceSize size, VkDeviceSize offset = std::numeric_limits<VkDeviceSize>::max());

		VULKAN_WRAPPER_API void write(const void * data, size_t sizeOfData, VkDeviceSize offset = 0, bool leaveMapped = true);
		VULKAN_WRAPPER_API inline void map(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0, VkMemoryMapFlags flags = 0);
		VULKAN_WRAPPER_API inline void flush();
		VULKAN_WRAPPER_API inline void invalidate();
		VULKAN_WRAPPER_API void copyFromBuffer(VkBuffer srcBuffer, VkBufferCopy copyRegion = {}, VkCommandPool commandPool = VK_NULL_HANDLE);
		//VULKAN_WRAPER_API void copyFrom(VkImage image, VkBufferCopy copyRegion, VkCommandPool commandPool = VK_NULL_HANDLE);

		VULKAN_WRAPPER_API VkDescriptorBufferInfo bufferInfo(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);
	private:
		VkDeviceSize size_m;
		VkDeviceSize offset_m;
		VkDeviceSize sizeInMemory_m;
		VkDeviceSize allignement_m;
		uint32_t memoryBits_m;

		MemoryRanges memoryRanges;
		Memory * memory = nullptr;

		friend void Memory::bindBufferToMemory(Buffer & buffer);
		friend SubBuffer;
	};


	// Sub Buffer
	class SubBuffer {
		friend SubBuffer;
		friend Buffer;
	public:
		VULKAN_WRAPPER_API SubBuffer();
		VULKAN_WRAPPER_API SubBuffer(const SubBuffer & rhs);
		VULKAN_WRAPPER_API ~SubBuffer();

		VULKAN_WRAPPER_API SubBuffer & operator = (const SubBuffer & rhs);
		VULKAN_WRAPPER_API operator VkBuffer () const;
		//VULKAN_WRAPPER_API Buffer & parent() const;

		VULKAN_WRAPPER_API void write(const void * data, size_t sizeOfData, bool leaveMapped = true);
		VULKAN_WRAPPER_API inline void copyFrom(const SubBuffer & srcBuffer, VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0, VkCommandPool commandPool = VK_NULL_HANDLE);
		VULKAN_WRAPPER_API inline void map(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0, VkMemoryMapFlags flags = 0);
		VULKAN_WRAPPER_API inline void flush();
		VULKAN_WRAPPER_API inline void invalidate();
		VULKAN_WRAPPER_API void clear();

		const VkDeviceSize & size;
		const VkDeviceSize & offset; // offset in buffer
	private:
		VkDeviceSize size_m;
		VkDeviceSize offset_m;

		SubBuffer(VkDeviceSize size, VkDeviceSize offset, Buffer * buffer);
		Buffer * buffer;
	};






	/// Image
	class Image : public impl::Object<impl::VkwImage> {
	public:
		struct CreateInfo : impl::CreateInfo {
			VkFormat                 format;
			VkImageUsageFlags        usage;
			VkExtent3D               extent;
			VkImageType              imageType = VK_IMAGE_TYPE_2D;
			uint32_t                 mipLevels = 1;
			VkImageLayout            initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			VkSampleCountFlagBits    samples = VK_SAMPLE_COUNT_1_BIT;
			uint32_t                 arrayLayers = 1;
			VkImageTiling            tiling = VK_IMAGE_TILING_OPTIMAL;
			VkSharingMode            sharingMode = VK_SHARING_MODE_EXCLUSIVE;
			std::vector<uint32_t>	 familyQueueIndicies;
			VkImageCreateFlags       flags = 0;
		};

		VULKAN_WRAPPER_API Image();
		VULKAN_WRAPPER_API Image(const CreateInfo & createInfo);
		VULKAN_WRAPPER_API Image(VkExtent3D extent, VkFormat format, VkImageUsageFlags usage, VkImageType imageType = VK_IMAGE_TYPE_2D, VkImageCreateFlags flags = 0);
		VULKAN_WRAPPER_API ~Image() = default;

		VULKAN_WRAPPER_API void createImage(const CreateInfo & createInfo);
		VULKAN_WRAPPER_API void createImage(VkExtent3D extent, VkFormat format, VkImageUsageFlags usage, VkImageType imageType = VK_IMAGE_TYPE_2D, VkImageCreateFlags flags = 0);

		VULKAN_WRAPPER_API Image & operator = (const Image & rhs);

		const VkImageLayout & layout;
		const VkExtent3D & extent;
		const VkDeviceSize & sizeInMemory;
		const uint32_t & memoryTypeBits;

		VkImageCreateFlags flags = 0;
		VkImageType imageType = VK_IMAGE_TYPE_2D;
		VkSharingMode sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		VkSampleCountFlagBits samples = VK_SAMPLE_COUNT_1_BIT;
		uint32_t mipLevels = 1;
		uint32_t arrayLayers = 1;
		VkImageTiling tiling = VK_IMAGE_TILING_OPTIMAL;
		VkFormat format;
		VkImageUsageFlags usage;
		std::vector<uint32_t> familyQueueIndicies;

		VULKAN_WRAPPER_API void transitionImageLayout(VkImageLayout newLayout, VkImageAspectFlags aspectMask, VkPipelineStageFlags srcStageMask = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, VkPipelineStageFlags dstStageMask = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, VkCommandPool commandPool = VK_NULL_HANDLE);
		VULKAN_WRAPPER_API void transitionImageLayout(VkImageLayout newLayout, const VkImageSubresourceRange & range, VkPipelineStageFlags srcStageMask = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, VkPipelineStageFlags dstStageMask = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, VkCommandPool commandPool = VK_NULL_HANDLE);

		VULKAN_WRAPPER_API void copyFromImage(const Image & srcImage, const std::vector<VkImageCopy> & regions = {}, VkCommandPool cmdPool = VK_NULL_HANDLE);
		VULKAN_WRAPPER_API void copyFromBuffer(const VkBuffer & srcBuffer, const std::vector<VkBufferImageCopy> & copyRegions = {}, VkCommandPool commandPool = VK_NULL_HANDLE, VkQueue queue = VK_NULL_HANDLE);
		// TODO: make function to copy from an image
		// TODO: transitionImageLayout should be redone
	private:
		VkImageLayout layout_m = VK_IMAGE_LAYOUT_UNDEFINED;
		VkExtent3D extent_m;
		VkDeviceSize size_m;
		uint32_t memoryTypeBits_m;

		friend void Memory::setMemoryTypeBitsImage(Image & image);
		friend void Memory::bindImageToMemory(Image & image);
		Memory * memory = nullptr; 
	};






	/// Image View
	class ImageView : public impl::Object<impl::VkwImageView> {
	public:
		struct CreateInfo : impl::CreateInfo {
			Image image;
			VkImageViewType viewType;
			VkImageSubresourceRange subresourceRange;
			VkComponentMapping components = {};
		};

		VULKAN_WRAPPER_API ImageView() = default;
		VULKAN_WRAPPER_API ImageView(const CreateInfo & createInfo);
		VULKAN_WRAPPER_API ImageView(const Image & image, VkImageSubresourceRange subresource, VkImageViewType viewType = VK_IMAGE_VIEW_TYPE_2D, VkComponentMapping components = {});
		VULKAN_WRAPPER_API ~ImageView() = default;

		VULKAN_WRAPPER_API void createImageView(const CreateInfo & createInfo);
		VULKAN_WRAPPER_API void createImageView(const Image & image, VkImageSubresourceRange subresource, VkImageViewType viewType = VK_IMAGE_VIEW_TYPE_2D, VkComponentMapping components = {});

		Image image;
		VkImageViewType viewType = VK_IMAGE_VIEW_TYPE_2D;
		VkImageSubresourceRange subresource;
		VkComponentMapping components = {};
	};






	/// Sampler
	class Sampler : public impl::Object<impl::VkwSampler> {
	public:
		struct CreateInfo : impl::CreateInfo {
			VkFilter magFilter = VK_FILTER_LINEAR;
			VkFilter minFilter = VK_FILTER_LINEAR;
			VkSamplerAddressMode addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
			VkSamplerAddressMode addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
			VkSamplerAddressMode addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
			VkSamplerMipmapMode mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
			float minLod = 0.0f;
			float maxLod = 0.0f;
			float mipLodBias = 0.0f;
			VkBorderColor borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;
			VkBool32 anisotropyEnable = VK_FALSE;
			float maxAnisotropy = 16;
			VkBool32 unnormalizedCoordinates = VK_FALSE;
			VkBool32 compareEnable = VK_FALSE;
			VkCompareOp compareOp = VK_COMPARE_OP_ALWAYS;
		};

		VULKAN_WRAPPER_API Sampler() = default;
		VULKAN_WRAPPER_API Sampler(const CreateInfo & createInfo);
		VULKAN_WRAPPER_API ~Sampler() = default;

		VULKAN_WRAPPER_API void createSampler(const CreateInfo & createInfo);
		VULKAN_WRAPPER_API void createSampler();

		VkBorderColor borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;
		VkBool32 anisotropyEnable = VK_FALSE;
		float maxAnisotropy = 16;
		VkBool32 unnormalizedCoordinates = VK_FALSE;
		VkBool32 compareEnable = VK_FALSE;
		VkCompareOp compareOp = VK_COMPARE_OP_ALWAYS;
	};






	/// Frame Buffer
	class FrameBuffer : public impl::Object<impl::VkwFramebuffer> {
	public:
		struct CreateInfo : impl::CreateInfo {
			std::vector<VkImageView> attachments;
			VkExtent2D extent;
			VkFramebufferCreateFlags flags = 0;
			VkRenderPass renderPass;
			uint32_t layers = 1;
		};

		VULKAN_WRAPPER_API FrameBuffer() = default;
		VULKAN_WRAPPER_API FrameBuffer(const CreateInfo & createInfo);
		VULKAN_WRAPPER_API FrameBuffer(VkRenderPass renderPass, VkExtent2D extent, std::vector<VkImageView> attachments, uint32_t layers = 1, VkFramebufferCreateFlags flags = 0);
		VULKAN_WRAPPER_API ~FrameBuffer() = default;

		VULKAN_WRAPPER_API void createFrameBuffer(const CreateInfo & createInfo);
		VULKAN_WRAPPER_API void createFrameBuffer(VkRenderPass renderPass, VkExtent2D extent, std::vector<VkImageView> attachments, uint32_t layers = 1, VkFramebufferCreateFlags flags = 0);

		VkFramebufferCreateFlags flags = 0;
		VkRenderPass renderPass;
		std::vector<VkImageView> attachments;
		VkExtent2D extent;
		uint32_t layers = 1;
	};
	
}
