#pragma once
#include <vulkan_wrapper.h>
#include <gli/gli.hpp>


namespace vkex {
	class Texture {
	public:
		VkDescriptorImageInfo descriptorInfo();
		vkw::Image image;
		vkw::ImageView imageView;
		vkw::Sampler sampler;
	};

	class Texture2D : public Texture {
	public:
		struct CreateInfo {
			CreateInfo() = default;
			CreateInfo(const std::string & fileName, VkFormat format, Texture2D * pTexture) : fileName(fileName), format(format), pTexture(pTexture) {}
			std::string fileName;
			Texture2D * pTexture;
			VkFormat format;
			VkImageUsageFlags usage = VK_IMAGE_USAGE_SAMPLED_BIT;
			VkImageUsageFlags imageUsageFlags = VK_IMAGE_USAGE_SAMPLED_BIT;
			VkImageLayout imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		};
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

	class TextureLoader {
	public:
		TextureLoader(const vkw::PhysicalDevice & pyhsicalDevice);
		void loadFromFile(const std::vector<Texture2D::CreateInfo> & createInfos);
		void loadFromFile(std::vector<TextureCube::CreateInfo> & createInfos);
		void loadFromFile(std::vector<TextureArray::CreateInfo> & createInfos);
		void setDefaultAllocSize(VkDeviceSize size);
	private:
		const vkw::PhysicalDevice & pyhsicalDevice;
		VkDeviceSize defaultAllocSize = 0;
		std::vector<vkw::Memory> allocations;
	};
}