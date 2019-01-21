#pragma once
#include "vkw_Include.h"
#include "vkw_Core.h"

namespace vkw {
	//class Window;
	class Swapchain;

	//add a CreateSurface


	//add a createSwapchain
	class Swapchain : public impl::Entity<impl::VkwSwapchainKHR> {
	public:
		struct CreateInfo {
			Surface & surface;
			// more preferences should be added
			VkSurfaceFormatKHR format;
			VkPresentModeKHR presentMode;
			VkExtent2D extent;
			uint32_t desiredImageCount;
		};

		VULKAN_WRAPER_API Swapchain();
		VULKAN_WRAPER_API Swapchain(Surface & surface);
		VULKAN_WRAPER_API Swapchain(const CreateInfo & createInfo);
		VULKAN_WRAPER_API ~Swapchain();

		VULKAN_WRAPER_API void createSwapchain(Surface & surface);
		VULKAN_WRAPER_API void createSwapchain(const CreateInfo & createInfo);

		VULKAN_WRAPER_API Swapchain & operator = (const Swapchain & rhs);

		VULKAN_WRAPER_API uint32_t getNextImage(VkSemaphore semaphore = VK_NULL_HANDLE, VkFence fence = VK_NULL_HANDLE);
		VULKAN_WRAPER_API void presentImage(uint32_t imageIndex, std::vector<VkSemaphore> semaphores);
		VULKAN_WRAPER_API VkImageView imageView(uint32_t i);

		const VkSurfaceFormatKHR & surfaceFormat;
		const VkPresentModeKHR & presentMode;
		const VkExtent2D & extent;
		const uint32_t & imageCount;

	private:
		VkSurfaceFormatKHR surfaceFormat_m;
		VkPresentModeKHR presentMode_m;
		VkExtent2D extent_m;
		uint32_t imageCount_m;

		std::vector<VkImage> swapChainImages;
		std::vector<VkImageView> swapChainImageViews;

		VkSurfaceFormatKHR chooseSwapchainSurfaceFormat(const std::vector<VkSurfaceFormatKHR> & availableFormats);
		VkPresentModeKHR chooseSwapchainPresentMode(const std::vector<VkPresentModeKHR> &availablePresentModes);
	};




	class Semaphore : public impl::Entity<impl::VkwSemaphore> {
	public:
		struct CreateInfo {
			VkSemaphoreCreateFlags flags = 0;
		};

		VULKAN_WRAPER_API Semaphore() = default;
		VULKAN_WRAPER_API Semaphore(const CreateInfo & createInfo);
		VULKAN_WRAPER_API Semaphore(VkSemaphoreCreateFlags flags);
		VULKAN_WRAPER_API ~Semaphore() = default;

		VULKAN_WRAPER_API void createSemaphore(const CreateInfo & createInfo);
		VULKAN_WRAPER_API void createSemaphore(VkSemaphoreCreateFlags flags = 0);

		VkSemaphoreCreateFlags flags;
	};




	class Fence : public impl::Entity<impl::VkwFence> {
	public:
		struct CreateInfo {
			VkSemaphoreCreateFlags flags = 0;
		};

		VULKAN_WRAPER_API Fence() = default;
		VULKAN_WRAPER_API Fence(const CreateInfo & createInfo);
		VULKAN_WRAPER_API Fence(VkFenceCreateFlags flags);
		VULKAN_WRAPER_API ~Fence() = default;

		VULKAN_WRAPER_API void createFence(const CreateInfo & createInfo);
		VULKAN_WRAPER_API void createFence(VkFenceCreateFlags flags = 0);

		VkFenceCreateFlags flags;

		VULKAN_WRAPER_API void wait(bool reset = true, uint64_t timeOut = std::numeric_limits<uint64_t>::max());
		VULKAN_WRAPER_API void reset();

		VULKAN_WRAPER_API static void reset(std::vector<Fence> & fences);
	};
	



	class RenderPass : public impl::Entity<impl::VkwRenderPass>{
	public:
		struct CreateInfo {
			std::vector<VkSubpassDescription> subPasses;
			std::vector<VkSubpassDependency> dependencys;
			std::vector<VkAttachmentDescription> attachements;
			VkRenderPassCreateFlags flags = 0;
		};

		VULKAN_WRAPER_API RenderPass() = default;
		VULKAN_WRAPER_API RenderPass(const CreateInfo & createInfo);
		VULKAN_WRAPER_API ~RenderPass() = default;

		VULKAN_WRAPER_API void createRenderPass(const CreateInfo & createInfo);
		
		std::vector<VkSubpassDescription> subPassDescriptions;
		std::vector<VkSubpassDependency> subPassDependencys;
		std::vector<VkAttachmentDescription> attachementsDescriptions;
		VkRenderPassCreateFlags flags = 0;
	};




	class ShaderModule : public impl::Entity<impl::VkwShaderModule> {
	public:
		struct CreateInfo {
			std::string filename; 
			VkShaderStageFlagBits stage;
			VkShaderModuleCreateFlags flags = 0;
		};

		VULKAN_WRAPER_API ShaderModule() = default;
		VULKAN_WRAPER_API ShaderModule(const CreateInfo & createInfo);
		VULKAN_WRAPER_API ShaderModule(std::string filename, VkShaderStageFlagBits stage, VkShaderModuleCreateFlags flags = 0);
		VULKAN_WRAPER_API ~ShaderModule() = default;

		VULKAN_WRAPER_API void createShaderModule(const CreateInfo & createInfo);
		VULKAN_WRAPER_API void createShaderModule(std::string filename, VkShaderStageFlagBits stage, VkShaderModuleCreateFlags flags = 0);

		const char * filename;
		VkShaderStageFlagBits stage;
		VkShaderModuleCreateFlags flags = 0;

		VULKAN_WRAPER_API VkPipelineShaderStageCreateInfo pipelineShaderStageCreateInfo(const VkSpecializationInfo* specializationInfo = nullptr, const char * name = "main");
	};




	class PipelineLayout : public impl::Entity<impl::VkwPipelineLayout>{
	public:
		struct CreateInfo {
			std::vector<VkDescriptorSetLayout> setLayouts;
			std::vector<VkPushConstantRange> pushConstants;
		};

		VULKAN_WRAPER_API PipelineLayout() = default;
		VULKAN_WRAPER_API PipelineLayout(const CreateInfo & createInfo);
		VULKAN_WRAPER_API PipelineLayout(const std::vector<VkDescriptorSetLayout> & setLayouts, const std::vector<VkPushConstantRange> & pushConstants);
		VULKAN_WRAPER_API ~PipelineLayout() = default;

		void createPipelineLayout(const CreateInfo & createInfo);
		void createPipelineLayout(const std::vector<VkDescriptorSetLayout> & setLayouts, const std::vector<VkPushConstantRange> & pushConstants);

		std::vector<VkDescriptorSetLayout> descriptorSetLayouts;
		std::vector<VkPushConstantRange> pushConstantRanges;
                                                                                                                                                        
	};




	class PipelineCache : public impl::Entity<impl::VkwPipelineCache> {
	public:
		struct CreateInfo {
			size_t size;
			void* data;
			VkPipelineCacheCreateFlags flags = 0;
		};

		VULKAN_WRAPER_API PipelineCache() = default;
		VULKAN_WRAPER_API PipelineCache(const CreateInfo & createInfo);
		VULKAN_WRAPER_API PipelineCache(size_t size, void* data, VkPipelineCacheCreateFlags flags = 0);
		VULKAN_WRAPER_API ~PipelineCache() = default;

		VkPipelineCacheCreateFlags flags = 0;
		size_t size;
		void* data;

		VULKAN_WRAPER_API void createPipelineCache();
	};




	class GraphicsPipeline : public impl::Entity<impl::VkwPipeline>{
	public:
		struct GraphicsPipelineStates {
			VkPipelineVertexInputStateCreateInfo	  vertexInputState	= { VK_STRUCTURE_TYPE_MAX_ENUM };
			VkPipelineInputAssemblyStateCreateInfo    inputAssemblyState = { VK_STRUCTURE_TYPE_MAX_ENUM };
			VkPipelineTessellationStateCreateInfo     tessellationState = { VK_STRUCTURE_TYPE_MAX_ENUM };
			VkPipelineViewportStateCreateInfo         viewportState = { VK_STRUCTURE_TYPE_MAX_ENUM };
			VkPipelineRasterizationStateCreateInfo    rasterizationState = { VK_STRUCTURE_TYPE_MAX_ENUM };
			VkPipelineMultisampleStateCreateInfo      multisampleState = { VK_STRUCTURE_TYPE_MAX_ENUM };
			VkPipelineDepthStencilStateCreateInfo     depthStencilState = { VK_STRUCTURE_TYPE_MAX_ENUM };
			VkPipelineColorBlendStateCreateInfo       colorBlendState = { VK_STRUCTURE_TYPE_MAX_ENUM };
			VkPipelineDynamicStateCreateInfo          dynamicState = { VK_STRUCTURE_TYPE_MAX_ENUM };
		};


		struct CreateInfo {
			GraphicsPipelineStates pipelineStates;
			std::vector<VkPipelineShaderStageCreateInfo> shaderStages;
			VkPipelineLayout layout;
			VkRenderPass renderPass;
			uint32_t subPass;
			VkPipelineCreateFlags flags = 0;
			VkPipelineCache cache = VK_NULL_HANDLE;
			VkPipeline basePipelineHandle = VK_NULL_HANDLE;
			int32_t basePipelineIndex = -1;
		};


		VULKAN_WRAPER_API GraphicsPipeline() = default;
		VULKAN_WRAPER_API GraphicsPipeline(CreateInfo & createInfo);

		std::vector<VkPipelineShaderStageCreateInfo>	shaderStages;
		VkPipelineLayout								layout;
		VkRenderPass									renderPass;
		uint32_t										subPass;
		GraphicsPipelineStates							pipelineStates = {};
		VkPipelineCreateFlags							flags = 0;
		VkPipeline										basePipelineHandle = VK_NULL_HANDLE;
		int32_t											basePipelineIndex = -1;
		VkPipelineCache									cache = VK_NULL_HANDLE;

		VULKAN_WRAPER_API void createPipeline(const CreateInfo & createInfo);
	};




	class ComputePipeline : public vkw::impl::Entity<impl::VkwPipeline> {
	public:
		/*VULKAN_WRAPER_API ComputePipeline() = default;
		VULKAN_WRAPER_API ~ComputePipeline() = default;*/
	};
}