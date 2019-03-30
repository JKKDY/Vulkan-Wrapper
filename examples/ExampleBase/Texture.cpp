#include "Texture.h"


namespace example {
	VkDescriptorImageInfo Texture::descriptorInfo()
	{
		VkDescriptorImageInfo info = {};
		info.imageLayout = image.layout;
		info.imageView = imageView;
		info.sampler = sampler;
		return info;
	}


	TextureLoader::TextureLoader(const vkw::PhysicalDevice & pyhsicalDevice):pyhsicalDevice(pyhsicalDevice){}

	void TextureLoader::loadFromFile(const std::vector<Texture2D::CreateInfo> & createInfos)
	{
		struct LoadInfo {
			LoadInfo(const gli::texture & texture) : gliTexture(texture) {}
			Texture2D * texture;
			gli::texture2d gliTexture;
			vkw::SubBuffer subStagingBuffer;
		};

		std::vector<LoadInfo> loadInfos;
		loadInfos.reserve(createInfos.size());
		size_t totalStagingSize = 0;
		size_t totalAllocationsSize = 0;

		for (auto & x : createInfos) {
			loadInfos.emplace_back(gli::load(x.fileName.c_str()));
			LoadInfo & info = loadInfos.back();
			info.texture = x.pTexture;
			totalStagingSize += info.gliTexture.size();

			vkw::Image::CreateInfo createInfo;
			createInfo.format = x.format;
			createInfo.usage = x.usage;
			if (!(createInfo.usage & VK_IMAGE_USAGE_TRANSFER_DST_BIT)) createInfo.usage |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;
			createInfo.extent = { static_cast<uint32_t>(info.gliTexture.extent().x), static_cast<uint32_t>(info.gliTexture.extent().y), 1 };
			createInfo.mipLevels = static_cast<uint32_t>(info.gliTexture.levels());

			info.texture->image.createImage(createInfo);
			totalAllocationsSize += info.texture->image.sizeInMemory;
		}



		bool needToAllocate = true;
		for (auto & x : allocations) {
			if (x.memoryRanges.query(totalAllocationsSize)) {
				for (LoadInfo & k : loadInfos) x.bindImageToMemory(k.texture->image);
				needToAllocate = false;
			}
		}

		if (needToAllocate) {
			std::vector<std::reference_wrapper<vkw::Image>> images;
			for (auto & x : loadInfos) images.push_back(x.texture->image);
			vkw::Memory::AllocInfo allocInfo = {};
			allocInfo.images = images;
			allocInfo.memoryFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
			allocInfo.additionalSize = defaultAllocSize == 0 ? 0 : std::max(defaultAllocSize - totalAllocationsSize, (unsigned long long)0);
			allocations.emplace_back(allocInfo);
		}



		vkw::Memory stagingMemory(VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
		vkw::Buffer stagingBuffer(VK_BUFFER_USAGE_TRANSFER_SRC_BIT, totalStagingSize);
		stagingMemory.allocateMemory({ stagingBuffer });

		for (auto & x : loadInfos) {
			x.subStagingBuffer = stagingBuffer.createSubBuffer(x.gliTexture.size());
			x.subStagingBuffer.write(x.gliTexture.data(), x.gliTexture.size());
			x.texture->image.transitionImageLayout(VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_ASPECT_COLOR_BIT);
		}



		for (auto & x : loadInfos) {
			std::vector<VkBufferImageCopy> bufferCopyRegions;
			size_t offset = x.subStagingBuffer.offset;
			for (uint32_t i = 0; i < x.gliTexture.levels(); i++)
			{
				VkBufferImageCopy bufferCopyRegion = {};
				bufferCopyRegion.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
				bufferCopyRegion.imageSubresource.mipLevel = i;
				bufferCopyRegion.imageSubresource.baseArrayLayer = 0;
				bufferCopyRegion.imageSubresource.layerCount = 1;
				bufferCopyRegion.imageExtent.width = static_cast<uint32_t>(x.gliTexture[i].extent().x);
				bufferCopyRegion.imageExtent.height = static_cast<uint32_t>(x.gliTexture[i].extent().y);
				bufferCopyRegion.imageExtent.depth = 1;
				bufferCopyRegion.bufferOffset = offset;

				bufferCopyRegions.push_back(bufferCopyRegion);

				offset += static_cast<size_t>(x.gliTexture[i].size());
			}

			x.texture->image.copyFromBuffer(x.subStagingBuffer, bufferCopyRegions);
			x.texture->image.transitionImageLayout(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VK_IMAGE_ASPECT_COLOR_BIT);
			x.subStagingBuffer.clear();

			vkw::Sampler::CreateInfo samplerCreateInfo = {};
			samplerCreateInfo.magFilter = VK_FILTER_LINEAR;
			samplerCreateInfo.minFilter = VK_FILTER_LINEAR;
			samplerCreateInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
			samplerCreateInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
			samplerCreateInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
			samplerCreateInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
			samplerCreateInfo.mipLodBias = 0.0f;
			samplerCreateInfo.compareOp = VK_COMPARE_OP_NEVER;
			samplerCreateInfo.minLod = 0.0f;
			samplerCreateInfo.maxLod = static_cast<float>(x.gliTexture.levels());
			samplerCreateInfo.maxAnisotropy = pyhsicalDevice.properties.limits.maxSamplerAnisotropy;
			samplerCreateInfo.anisotropyEnable = VK_TRUE;
			samplerCreateInfo.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;
			x.texture->sampler.createSampler(samplerCreateInfo);

			vkw::ImageView::CreateInfo viewCreateInfo = {};
			viewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
			viewCreateInfo.components = { VK_COMPONENT_SWIZZLE_R, VK_COMPONENT_SWIZZLE_G, VK_COMPONENT_SWIZZLE_B, VK_COMPONENT_SWIZZLE_A };
			viewCreateInfo.subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };
			viewCreateInfo.subresourceRange.levelCount = static_cast<uint32_t>(x.gliTexture.levels());
			viewCreateInfo.image = x.texture->image;
			x.texture->imageView.createImageView(viewCreateInfo);
		}
	}
	void TextureLoader::setDefaultAllocSize(VkDeviceSize size)
	{
		defaultAllocSize = size;
	}
}