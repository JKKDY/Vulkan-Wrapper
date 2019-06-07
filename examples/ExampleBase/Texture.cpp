#include "Texture.h"
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace vkx {

	VkDescriptorImageInfo Texture::descriptorInfo()
	{
		VkDescriptorImageInfo info = {};
		info.imageLayout = image.layout;
		info.imageView = imageView;
		info.sampler = sampler;
		return info;
	}


	void Texture2D::loadFromFile(const std::string & filename, VkFormat format, VkImageUsageFlags usage, vkw::Sampler::CreateInfo * pSamplerCreateInfo)
	{
		// load and create Image
		struct LoadInfo {
			gli::texture2d gliTexture;
			stbi_uc * pixels = nullptr;
			size_t size;
		}loadInfo;

		vkw::Image::CreateInfo imageCreateInfo;
		imageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
		imageCreateInfo.format = format;
		imageCreateInfo.usage = usage;
		// Ensure that the TRANSFER_DST bit is set for staging
		if (!(imageCreateInfo.usage & VK_IMAGE_USAGE_TRANSFER_DST_BIT))
		{
			imageCreateInfo.usage |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;
		}

		std::string fileExt = vkw::tools::getFileExtension(filename);

		if (fileExt == "dds" || fileExt == "ktx") {	// load with gli
			loadInfo.gliTexture = gli::texture2d(gli::load(filename.c_str()));
			loadInfo.size = loadInfo.gliTexture.size();

			imageCreateInfo.mipLevels = static_cast<uint32_t>(loadInfo.gliTexture.levels());
			imageCreateInfo.extent = { static_cast<uint32_t>(loadInfo.gliTexture.extent().x), static_cast<uint32_t>(loadInfo.gliTexture.extent().y), 1 };
		}
		else {	// load with stb
			int texWidth, texHeight, texChannels;
			loadInfo.pixels = stbi_load(filename.c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);

			VKW_assert(loadInfo.pixels != nullptr, "Failed to load imgae");
			loadInfo.size = 4 * texWidth * texHeight;

			imageCreateInfo.extent = { (uint32_t)texWidth , (uint32_t)texHeight, 1 };
			imageCreateInfo.mipLevels = 1;
		}

		
		image.createImage(imageCreateInfo);
		memory.allocateMemory(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, {}, { image });
		image.transitionImageLayout(VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_ASPECT_COLOR_BIT);



		// setup staging buffer
		vkw::Memory stagingMemory(VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
		vkw::Buffer stagingBuffer(VK_BUFFER_USAGE_TRANSFER_SRC_BIT, image.sizeInMemory);
		stagingMemory.allocateMemory({ stagingBuffer });

		if (loadInfo.pixels == nullptr)
			stagingBuffer.write(loadInfo.gliTexture.data(), loadInfo.gliTexture.size());
		else
			stagingBuffer.write(loadInfo.pixels, loadInfo.size);

		 
		// write to image
		std::vector<VkBufferImageCopy> bufferCopyRegions;

		if (loadInfo.pixels == nullptr) {
			size_t offset = 0;
			for (uint32_t i = 0; i < image.mipLevels; i++)
			{
				VkBufferImageCopy bufferCopyRegion = {};
				bufferCopyRegion.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
				bufferCopyRegion.imageSubresource.mipLevel = i;
				bufferCopyRegion.imageSubresource.baseArrayLayer = 0;
				bufferCopyRegion.imageSubresource.layerCount = 1;
				bufferCopyRegion.imageExtent.width = static_cast<uint32_t>(loadInfo.gliTexture[i].extent().x);
				bufferCopyRegion.imageExtent.height = static_cast<uint32_t>(loadInfo.gliTexture[i].extent().y);
				bufferCopyRegion.imageExtent.depth = 1;
				bufferCopyRegion.bufferOffset = offset;

				bufferCopyRegions.push_back(bufferCopyRegion);

				offset += static_cast<size_t>(loadInfo.gliTexture[i].size());
			}
		}
		else {
			VkBufferImageCopy bufferCopyRegion = {};
			bufferCopyRegion.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			bufferCopyRegion.imageSubresource.mipLevel = 0;
			bufferCopyRegion.imageSubresource.baseArrayLayer = 0;
			bufferCopyRegion.imageSubresource.layerCount = 1;
			bufferCopyRegion.imageExtent = image.extent;
			bufferCopyRegion.bufferOffset = 0;
			bufferCopyRegions.push_back(bufferCopyRegion);
		}

		image.copyFromBuffer(stagingBuffer, bufferCopyRegions);
		image.transitionImageLayout(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VK_IMAGE_ASPECT_COLOR_BIT);


		// setup sampler and image view
		vkw::Sampler::CreateInfo samplerCreateInfo = {};
		if (pSamplerCreateInfo == nullptr) {
			samplerCreateInfo.magFilter = VK_FILTER_LINEAR;
			samplerCreateInfo.minFilter = VK_FILTER_LINEAR;
			samplerCreateInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
			samplerCreateInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
			samplerCreateInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
			samplerCreateInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
			samplerCreateInfo.mipLodBias = 0.0f;
			samplerCreateInfo.compareOp = VK_COMPARE_OP_NEVER;
			samplerCreateInfo.minLod = 0.0f;
			samplerCreateInfo.maxLod = (float)image.mipLevels;
			samplerCreateInfo.maxAnisotropy = 0;
			samplerCreateInfo.anisotropyEnable = VK_FALSE;
			samplerCreateInfo.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;
		}
		else {
			samplerCreateInfo = *pSamplerCreateInfo;
		}
		sampler.createSampler(samplerCreateInfo);


		vkw::ImageView::CreateInfo viewCreateInfo = {};
		viewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		viewCreateInfo.components = { VK_COMPONENT_SWIZZLE_R, VK_COMPONENT_SWIZZLE_G, VK_COMPONENT_SWIZZLE_B, VK_COMPONENT_SWIZZLE_A };
		viewCreateInfo.subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };
		viewCreateInfo.subresourceRange.levelCount = image.mipLevels;
		viewCreateInfo.image = image;
		imageView.createImageView(viewCreateInfo);
	}


	void TextureCube::loadFromFile(const std::string & filename, VkFormat format, VkImageUsageFlags usage, vkw::Sampler::CreateInfo * pSamplerCreateInfo)
	{
		gli::texture_cube texCube(gli::load(filename));

		std::vector<VkBufferImageCopy> bufferCopyRegions;
		size_t offset = 0;

		for (uint32_t face = 0; face < 6; face++) {
			for (uint32_t level = 0; level < texCube.levels(); level++) {
				VkBufferImageCopy bufferCopyRegion = {};
				bufferCopyRegion.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
				bufferCopyRegion.imageSubresource.mipLevel = level;
				bufferCopyRegion.imageSubresource.baseArrayLayer = face;
				bufferCopyRegion.imageSubresource.layerCount = 1;
				bufferCopyRegion.imageExtent.width = static_cast<uint32_t>(texCube[face][level].extent().x);
				bufferCopyRegion.imageExtent.height = static_cast<uint32_t>(texCube[face][level].extent().y);
				bufferCopyRegion.imageExtent.depth = 1;
				bufferCopyRegion.bufferOffset = offset;

				bufferCopyRegions.push_back(bufferCopyRegion);

				offset += texCube[face][level].size();
			}
		}


		vkw::Image::CreateInfo imageCreateInfo;
		imageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
		imageCreateInfo.format = format;
		imageCreateInfo.usage = usage;
		imageCreateInfo.extent = { static_cast<uint32_t>(texCube.extent().x) , static_cast<uint32_t>(texCube.extent().y), 1 };
		imageCreateInfo.mipLevels = static_cast<uint32_t>(texCube.levels());
		imageCreateInfo.flags = VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT;   // This flag is required for cube map images
		imageCreateInfo.arrayLayers = 6;	// Cube faces count as array layers in Vulkan
		// Ensure that the TRANSFER_DST bit is set for staging
		if (!(imageCreateInfo.usage & VK_IMAGE_USAGE_TRANSFER_DST_BIT))
		{
			imageCreateInfo.usage |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;
		}

		image.createImage(imageCreateInfo);
		image.transitionImageLayout(VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_ASPECT_COLOR_BIT);
		memory.allocateMemory(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, {}, { image });

		vkw::Memory stagingMemory(VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
		vkw::Buffer stagingBuffer(VK_BUFFER_USAGE_TRANSFER_SRC_BIT, image.sizeInMemory);
		stagingMemory.allocateMemory({ stagingBuffer });
		stagingBuffer.write(texCube.data(), texCube.size());

		image.copyFromBuffer(stagingBuffer, bufferCopyRegions);


		// setup sampler and image view
		vkw::Sampler::CreateInfo samplerCreateInfo = {};
		if (pSamplerCreateInfo == nullptr) {
			samplerCreateInfo.magFilter = VK_FILTER_LINEAR;
			samplerCreateInfo.minFilter = VK_FILTER_LINEAR;
			samplerCreateInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
			samplerCreateInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
			samplerCreateInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
			samplerCreateInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
			samplerCreateInfo.mipLodBias = 0.0f;
			samplerCreateInfo.compareOp = VK_COMPARE_OP_NEVER;
			samplerCreateInfo.minLod = 0.0f;
			samplerCreateInfo.maxLod = (float)image.mipLevels;
			samplerCreateInfo.maxAnisotropy = 0;
			samplerCreateInfo.anisotropyEnable = VK_FALSE;
			samplerCreateInfo.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;
		}
		else {
			samplerCreateInfo = *pSamplerCreateInfo;
		}
		sampler.createSampler(samplerCreateInfo);


		vkw::ImageView::CreateInfo viewCreateInfo = {};
		viewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_CUBE;
		viewCreateInfo.components = { VK_COMPONENT_SWIZZLE_R, VK_COMPONENT_SWIZZLE_G, VK_COMPONENT_SWIZZLE_B, VK_COMPONENT_SWIZZLE_A };
		viewCreateInfo.subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };
		viewCreateInfo.subresourceRange.levelCount = image.mipLevels;
		viewCreateInfo.subresourceRange.layerCount = image.arrayLayers; // 6
		viewCreateInfo.image = image;
		imageView.createImageView(viewCreateInfo);
	}
}