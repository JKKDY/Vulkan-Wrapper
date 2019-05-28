#pragma once
#include <vulkan_wrapper.h>
#include <gli/gli.hpp>


namespace vkx {
	class Texture {
	public:
		Texture() = default;
		VkDescriptorImageInfo descriptorInfo();
		vkw::Image image;
		vkw::ImageView imageView;
		vkw::Sampler sampler;
		vkw::Memory memory;
	};

	class Texture2D : public Texture {
	public:
		void loadFromFile(const std::string & filename, VkFormat format, VkImageUsageFlags usage = VK_IMAGE_USAGE_SAMPLED_BIT, vkw::Sampler::CreateInfo * pSamplerCreateInfo = nullptr);
	};

	class TextureCube : public Texture {
	public:
		struct CreateInfo {
			CreateInfo() = default;
			CreateInfo(const std::string & fileName, VkFormat format, Texture2D * TextureCube);
			std::string fileName;
			Texture2D * texture;
			VkFormat format;
			VkImageUsageFlags usage = VK_IMAGE_USAGE_SAMPLED_BIT;
			VkImageUsageFlags imageUsageFlags = VK_IMAGE_USAGE_SAMPLED_BIT;
			VkImageLayout imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		};
	};

	class TextureArray : public Texture {
	public:
		struct CreateInfo {
			std::string fileName;
			Texture2D * texture;
			VkFormat format;
			VkImageUsageFlags usage = VK_IMAGE_USAGE_SAMPLED_BIT;
			VkImageUsageFlags imageUsageFlags = VK_IMAGE_USAGE_SAMPLED_BIT;
			VkImageLayout imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		};
	};
}