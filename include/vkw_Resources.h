#pragma once
#include "vkw_Include.h"


namespace vkw {
	class Buffer;
	class Image;
	class SubBuffer;
    


	/// Descriptor Pool
	class DescriptorPool : public impl::Entity<impl::VkwDescriptorPool>{
		struct CreateInfo2 { // implement createFunction
			uint32_t SamplerCount = 0;
			uint32_t CombinedImageSamplerCount = 0;
			uint32_t SampledImageCount = 0;
			uint32_t StorageImagCount = 0;
			uint32_t UniformTexelBufferCount = 0;
			uint32_t StorageTexelBufferCount = 0;
			uint32_t UniformBufferCount = 0;
			uint32_t StorageBufferCount = 0;
			uint32_t UniformBufferDynamicCount = 0;
			uint32_t StorageBufferDynamicCount = 0;
			uint32_t maxSets;
		};
	public:
		struct CreateInfo {
			std::vector<VkDescriptorPoolSize> poolSizes;
			uint32_t maxSets;
			VkDescriptorPoolCreateFlags flags = 0;
		};

		VULKAN_WRAPER_API DescriptorPool();
		VULKAN_WRAPER_API DescriptorPool(const CreateInfo & createInfo);
		VULKAN_WRAPER_API DescriptorPool(const CreateInfo2 & createInfo);
		VULKAN_WRAPER_API DescriptorPool(const std::vector<VkDescriptorPoolSize> & poolSizes, uint32_t maxSets, VkDescriptorPoolCreateFlags flags = 0);

		VULKAN_WRAPER_API void createDescriptorPool(const CreateInfo & createInfo);
		VULKAN_WRAPER_API void createDescriptorPool(const CreateInfo2 & createInfo);
		VULKAN_WRAPER_API void createDescriptorPool(const std::vector<VkDescriptorPoolSize> & poolSizes, uint32_t maxSets, VkDescriptorPoolCreateFlags flags = 0);

		VULKAN_WRAPER_API DescriptorPool & operator = (const DescriptorPool & p);
		//VULKAN_WRAPER_API DescriptorPool & operator = (DescriptorPool && p);

		const std::vector<VkDescriptorPoolSize> & poolSizes;
		const uint32_t & maxSets;
		const VkDescriptorPoolCreateFlags & flags;

		VULKAN_WRAPER_API void resetDescriptorPool(VkDescriptorPoolResetFlags flags = 0);
	private:
		std::vector<VkDescriptorPoolSize> poolSizes_m;
		uint32_t maxSets_m;
		VkDescriptorPoolCreateFlags flags_m = 0;
	};





	/// Descriptor Set Layout
	class DescriptorSetLayout : public impl::Entity<impl::VkwDescriptorSetLayout> {
	public:
		VULKAN_WRAPER_API DescriptorSetLayout() = default;
		VULKAN_WRAPER_API DescriptorSetLayout(std::vector<VkDescriptorSetLayoutBinding> layoutBindings, VkDescriptorSetLayoutCreateFlags flags = 0);
		VULKAN_WRAPER_API ~DescriptorSetLayout() = default;

		std::map<uint32_t, VkDescriptorSetLayoutBinding> layoutBindings;
		VkDescriptorSetLayoutCreateFlags flags = 0;

		VULKAN_WRAPER_API void createDescriptorSetLayout();	
	};






	/// Descriptor Set
	class DescriptorSet : public impl::Entity<impl::VkwDescriptorSet> {
	public:
		struct WriteInfo {
			uint32_t dstBinding;
			uint32_t dstArrayElement;
			uint32_t descriptorCount;
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

		VULKAN_WRAPER_API DescriptorSet() = default;
		VULKAN_WRAPER_API DescriptorSet(VkDescriptorPool descriptorPool, DescriptorSetLayout & layout, bool allocateDescriptorSet = true);
		VULKAN_WRAPER_API ~DescriptorSet() = default;

		VkDescriptorPool descriptorPool;
		DescriptorSetLayout * layout = nullptr;

		VULKAN_WRAPER_API void allocateDescriptorSet();
		VULKAN_WRAPER_API void update(std::vector<WriteInfo> * writeInfos, std::vector<CopyInfo> * copyInfos); //TODO T003 implement copying
		VULKAN_WRAPER_API void write(uint32_t dstBinding, uint32_t dstArrayElement, uint32_t descriptorCount, const VkDescriptorImageInfo * pImageInfo = nullptr, const VkDescriptorBufferInfo * pBufferInfo = nullptr, const VkBufferView * pTexelBufferView = nullptr);
	};

	VULKAN_WRAPER_API void allocateDescriptorSets(std::vector<DescriptorSet&> descriptorSets);






	/// Memory
	class Memory : public impl::Entity<impl::VkwDeviceMemory> {
		friend Buffer;
		struct Mapped {
			VkDeviceSize size = 0;
			VkDeviceSize offset = 0;
			void * mapped = nullptr;
		};
	public:
		VULKAN_WRAPER_API Memory() = default;
		VULKAN_WRAPER_API Memory(VkMemoryPropertyFlags memoryFlags, VkDeviceSize size = 0);
		VULKAN_WRAPER_API ~Memory() {}

		VkMemoryPropertyFlags memoryFlags;
		uint32_t memoryTypeBits = std::numeric_limits<uint32_t>::max();
		uint32_t memoryType = std::numeric_limits<uint32_t>::max();
		VkDeviceSize size = 0;

		VULKAN_WRAPER_API void allocateMemory(std::initializer_list<std::reference_wrapper<Buffer>> buffers = {}, std::initializer_list<std::reference_wrapper<Image>> images = {});

		VULKAN_WRAPER_API void setMemoryTypeBitsBuffer(Buffer & buffer);
		VULKAN_WRAPER_API void bindBufferToMemory(Buffer & buffer);

		VULKAN_WRAPER_API void setMemoryTypeBitsImage(Image & image);
		VULKAN_WRAPER_API void bindImageToMemory(Image & image);

		VULKAN_WRAPER_API void setMemoryTypeBits(VkMemoryRequirements & memoryRequirements);
		VULKAN_WRAPER_API void * map(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0, VkMemoryMapFlags flags = 0);
		VULKAN_WRAPER_API void unMap();

		Mapped memoryMap;
	private:
		std::map<VkDeviceSize, VkDeviceSize> memoryRanges;
		uint32_t findMemoryType();
		static VkDeviceSize getOffset(VkDeviceSize & dataSize, VkDeviceSize maxSize, std::map<VkDeviceSize, VkDeviceSize> & memoryRanges, VkDeviceSize allignement = 1);
	};
	




	/// Buffer
	class Buffer : public impl::Entity<impl::VkwBuffer> {
		friend SubBuffer;
	public:
		struct CreateInfo {
			VkBufferUsageFlags usageFlags;
			VkBufferCreateFlags createflags;
			VkDeviceSize size; 
			VkSharingMode sharingMode = VK_SHARING_MODE_EXCLUSIVE;
			VkDeviceSize offset = 0;
		};

		VULKAN_WRAPER_API Buffer() = default;
		VULKAN_WRAPER_API Buffer(CreateInfo createInfo);
		VULKAN_WRAPER_API Buffer(VkBufferUsageFlags usageFlags, VkDeviceSize size, VkSharingMode sharingMode = VK_SHARING_MODE_EXCLUSIVE, VkDeviceSize offset = 0, VkBufferCreateFlags createflags = 0);
		VULKAN_WRAPER_API ~Buffer();

		VkBufferUsageFlags usageFlags;
		VkBufferCreateFlags createFlags;
		VkSharingMode sharingMode = VK_SHARING_MODE_EXCLUSIVE; 
		VkDeviceSize size;
		VkDeviceSize sizeInMemory; // Size is alligned with allignement e.g. "size" = usable size, "sizeInMemory" = size the buffer takes up in its Vk::Memory
		VkDeviceSize allignement;
		VkDeviceSize offset; // offset in Memory

		VULKAN_WRAPER_API void createBuffer();
		VULKAN_WRAPER_API SubBuffer createSubBuffer(VkDeviceSize size);

		VULKAN_WRAPER_API void write(const void * data, size_t sizeOfData, VkDeviceSize offset = 0, bool leaveMapped = true);
		VULKAN_WRAPER_API void copyFromBuffer(VkBuffer srcBuffer, VkBufferCopy copyRegion = {}, VkCommandPool commandPool = VK_NULL_HANDLE);
		//VULKAN_WRAPER_API void copyFrom(VkImage image, VkBufferCopy copyRegion, VkCommandPool commandPool = VK_NULL_HANDLE);
	private:
		friend void Memory::bindBufferToMemory(Buffer & buffer);
		std::map<VkDeviceSize, VkDeviceSize> memoryRanges;
		Memory * memory = nullptr;
	};



	/// Sub Buffer
	class SubBuffer {
		friend SubBuffer;
		friend Buffer;
	public:
		VULKAN_WRAPER_API ~SubBuffer();
		VULKAN_WRAPER_API void write(const void * data, size_t sizeOfData = 0, bool leaveMapped = false);
		VULKAN_WRAPER_API void copyFrom(SubBuffer & srcBuffer, VkCommandPool commandPool = VK_NULL_HANDLE);
		VULKAN_WRAPER_API void clear();

		const VkDeviceSize size;
		const VkDeviceSize offset;
	private:
		SubBuffer(VkDeviceSize size, VkDeviceSize offset, Buffer & buffer);
		Buffer & buffer;
	};






	/// Image
	class Image : public impl::Entity<impl::VkwImage> {
	public:
		VULKAN_WRAPER_API Image() = default;
		VULKAN_WRAPER_API Image(VkExtent3D extent, VkFormat format, VkImageUsageFlags usage, uint32_t miplvls = 1, uint32_t layers = 1, VkImageCreateFlags flags = 0);
		VULKAN_WRAPER_API ~Image() = default;

		VkImageCreateFlags       flags = 0;
		VkImageType              imageType = VK_IMAGE_TYPE_2D;
		VkImageLayout            layout = VK_IMAGE_LAYOUT_UNDEFINED;
		VkSharingMode            sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		VkSampleCountFlagBits    samples = VK_SAMPLE_COUNT_1_BIT;
		uint32_t                 mipLevels = 1;
		uint32_t                 arrayLayers = 1;
		VkImageTiling            tiling = VK_IMAGE_TILING_OPTIMAL;
		VkFormat                 format;
		VkExtent3D               extent;
		VkImageUsageFlags        usage;
		std::vector<uint32_t>	 familyQueueIndicies;

		VULKAN_WRAPER_API void createImage();
		VULKAN_WRAPER_API void transitionImageLayout(VkImageLayout newLayout, VkCommandPool commandPool = VK_NULL_HANDLE, VkImageSubresourceRange range = { 0x7FFFFFFF }, VkAccessFlags srcAccess = 0x7FFFFFFF, VkAccessFlags dstAccess = 0x7FFFFFFF);
		VULKAN_WRAPER_API void copyFromImage(Image & srcImage, std::vector<VkImageCopy> regions = {}, VkCommandPool cmdPool = VK_NULL_HANDLE);
		VULKAN_WRAPER_API void copyFromBuffer(Buffer & srcBuffer, std::vector<VkBufferImageCopy> copyRegions = {}, VkCommandPool commandPool = VK_NULL_HANDLE);
		// make fkt to copy from an image
	private:
		friend void Memory::bindImageToMemory(Image & image);
		Memory * memory = nullptr; 
	};






	/// Image View
	class ImageView : public impl::Entity<impl::VkwImageView> {
	public:
		VULKAN_WRAPER_API ImageView() = default;
		VULKAN_WRAPER_API ImageView(Image & image, VkImageSubresourceRange subresource, VkImageViewType viewType = VK_IMAGE_VIEW_TYPE_2D, VkComponentMapping components = {});
		VULKAN_WRAPER_API ~ImageView() = default;

		Image image;
		VkImageViewType viewType = VK_IMAGE_VIEW_TYPE_2D;
		VkImageSubresourceRange subresource;
		VkComponentMapping components = {};

		VULKAN_WRAPER_API void createImageView();
	};






	/// Sampler
	class Sampler : public impl::Entity<impl::VkwSampler> {
		struct Filter {
			Filter() = default;
			Filter(VkFilter filter) {
				magFilter = filter;
				minFilter = filter;
			}
			VkFilter magFilter = VK_FILTER_LINEAR;
			VkFilter minFilter = VK_FILTER_LINEAR;
		};

		struct AddressMode {
			AddressMode() = default;
			AddressMode(VkSamplerAddressMode addressMode) {
				addressModeU = addressMode;
				addressModeV = addressMode;
				addressModeW = addressMode;
			}
			VkSamplerAddressMode addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
			VkSamplerAddressMode addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
			VkSamplerAddressMode addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		};

		struct MipMap {
			VkSamplerMipmapMode mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
			float minLod = 0.0f;
			float maxLod = 0.0f;
			float mipLodBias = 0.0f;
		};
	public:
		VULKAN_WRAPER_API Sampler();
		VULKAN_WRAPER_API ~Sampler() = default;

		Filter filter {};
		AddressMode addressMode{};
		MipMap mipMap {};

		VkBorderColor borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;
		VkBool32 anisotropyEnable = VK_FALSE;
		float maxAnisotropy = 16;
		VkBool32 unnormalizedCoordinates = VK_FALSE;
		VkBool32 compareEnable = VK_FALSE;
		VkCompareOp compareOp = VK_COMPARE_OP_ALWAYS;
		
		VULKAN_WRAPER_API void createSampler();
	};






	/// Frame Buffer
	class FrameBuffer : public impl::Entity<impl::VkwFramebuffer> {
	public:
		VULKAN_WRAPER_API FrameBuffer() = default;
		VULKAN_WRAPER_API ~FrameBuffer() = default;

		VkFramebufferCreateFlags flags = 0;
		VkRenderPass renderPass;
		std::vector<VkImageView> attachments;
		VkExtent2D extent;
		uint32_t layers = 1;

		VULKAN_WRAPER_API void createFrameBuffer();
	};
	
}
