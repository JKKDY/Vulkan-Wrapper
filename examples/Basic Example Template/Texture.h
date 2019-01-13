#pragma once
#include <vulkan_wraper.h>
#include <initializer_list>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include "init.h"

namespace example {
	struct Texture {
		vkw::Image image;
		vkw::ImageView imageView;
		vkw::Sampler sampler;
		VkDeviceSize size;
		void * pixels;

		VkDescriptorImageInfo info() {
			VkDescriptorImageInfo info = {};
			info.imageLayout = image.layout;
			info.imageView = imageView;
			info.sampler = sampler;

			return info;
		}

		virtual void cleanUp() = 0;
	};



	struct Texture2D : public Texture {
		Texture2D() = default;
		Texture2D(std::string path) {
			int texWidth, texHeight, texChannels;
			pixels = stbi_load(path.c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
			size = texWidth * texHeight * 4;

			image = vkw::Image({ (uint32_t)texWidth, (uint32_t)texHeight }, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT);
		}

		void cleanUp() override {
			
		};
	};



	struct SolidColorTexture : public Texture {
		SolidColorTexture(std::array<uint8_t, 4> color) {
			image = vkw::Image({ 1, 1, 1 }, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT);
			size = 4;
			pixels = this->color.data();
		}

		void cleanUp() override {}
	private:
		std::array<uint8_t, 4> color;
	};





	struct TextureLoader {
		void loadTextures(std::initializer_list<std::reference_wrapper<Texture>> textures) {
			textureMemory = vkw::Memory(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

			VkDeviceSize maxSize = 0;

			for (Texture & x : textures) {
				textureMemory.setMemoryTypeBitsImage(x.image);
				if (x.size > maxSize) {
					maxSize = x.size;
				}
			}

			textureMemory.allocateMemory();

			vkw::Memory stagingMemory(VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
			vkw::Buffer stagingBuffer(VK_BUFFER_USAGE_TRANSFER_SRC_BIT, maxSize);
			stagingMemory.allocateMemory({ stagingBuffer });


			for (Texture & x : textures) {
				textureMemory.bindImageToMemory({ x.image });
				stagingBuffer.write(x.pixels, x.size);

				x.image.transitionImageLayout(VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

				VkBufferImageCopy region = {};
				region.bufferOffset = 0;
				region.bufferImageHeight = 0;
				region.bufferRowLength = 0;
				region.imageSubresource = imageSubresourceLayers(VK_IMAGE_ASPECT_COLOR_BIT);
				region.imageOffset = { 0,0,0 };
				region.imageExtent = x.image.extent;

				x.image.copyFromBuffer(stagingBuffer, { region });
				x.image.transitionImageLayout(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

				x.imageView = vkw::ImageView(x.image, imageSubresourceRange(VK_IMAGE_ASPECT_COLOR_BIT));
				x.sampler = vkw::Sampler();
				x.sampler.createSampler();

				x.cleanUp();
			}
		}

	private:
		vkw::Memory textureMemory;
	};
}