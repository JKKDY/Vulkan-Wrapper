#pragma once
#include "vkw_Config.h"

namespace vkw {
	namespace Init {
		inline VULKAN_WRAPER_API VkDebugUtilsMessengerCreateInfoEXT  debugUtilsMessengerCreateInfoEXT() {
			VkDebugUtilsMessengerCreateInfoEXT createInfo = {};
			createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
			return createInfo;
		}


		inline VULKAN_WRAPER_API VkApplicationInfo applicationInfo() {
			VkApplicationInfo appInfo = {};
			appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
			return appInfo;
		}


		inline VULKAN_WRAPER_API VkInstanceCreateInfo instanceCreateInfo() {
			VkInstanceCreateInfo createInfo = {};
			createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
			return createInfo;
		}


		inline VULKAN_WRAPER_API VkDeviceQueueCreateInfo deviceQueueCreateInfo() {
			VkDeviceQueueCreateInfo createInfo = {};
			createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			return createInfo;
		}


		inline VULKAN_WRAPER_API VkDeviceCreateInfo deviceCreateInfo() {
			VkDeviceCreateInfo createInfo = {}; 
			createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
			return createInfo;
		}


		inline VULKAN_WRAPER_API VkSwapchainCreateInfoKHR swapchainCreateInfoKHR() {
			VkSwapchainCreateInfoKHR createInfo = {}; 
			createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
			return createInfo;
		}


		inline VULKAN_WRAPER_API VkImageViewCreateInfo imageViewCreateInfo() {
			VkImageViewCreateInfo createInfo = {};
			createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			return createInfo;
		}


		inline VULKAN_WRAPER_API VkComponentMapping componentMappingSwizzleIdentity() {
			VkComponentMapping mapping = {
				VK_COMPONENT_SWIZZLE_IDENTITY,
				VK_COMPONENT_SWIZZLE_IDENTITY,
				VK_COMPONENT_SWIZZLE_IDENTITY,
				VK_COMPONENT_SWIZZLE_IDENTITY
			};
			return mapping;
		}


		inline VULKAN_WRAPER_API VkPresentInfoKHR presentInfoKHR() {
			VkPresentInfoKHR presentInfo = {};
			presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
			return presentInfo;
		}


		inline VULKAN_WRAPER_API VkSemaphoreCreateInfo semaphoreCreateInfo() {
			VkSemaphoreCreateInfo createInfo = {};
			createInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
			return createInfo;
		}


		inline VULKAN_WRAPER_API VkFenceCreateInfo fenceCreateInfo() {
			VkFenceCreateInfo createInfo = {};
			createInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
			return createInfo;
		}


		inline VULKAN_WRAPER_API VkRenderPassCreateInfo renderPassCreateInfo() {
			VkRenderPassCreateInfo createInfo = {};
			createInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
			return createInfo;
		}


		inline VULKAN_WRAPER_API VkShaderModuleCreateInfo shaderModuleCreateInfo() {
			VkShaderModuleCreateInfo createInfo = {};
			createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
			return createInfo;
		}


		inline VULKAN_WRAPER_API VkPipelineShaderStageCreateInfo pipelineShaderStageCreateInfo() {
			VkPipelineShaderStageCreateInfo shaderStageInfo = {};
			shaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
			shaderStageInfo.pName = "main";
			return shaderStageInfo;
		}


		inline VULKAN_WRAPER_API VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo() {
			VkPipelineLayoutCreateInfo createInfo = {};
			createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
			return createInfo;
		}


		inline VULKAN_WRAPER_API VkPipelineCacheCreateInfo pipelineCacheCreateInfo() {
			VkPipelineCacheCreateInfo createInfo = {};
			createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;
			return createInfo;
		}


		inline VULKAN_WRAPER_API VkPipelineVertexInputStateCreateInfo pipelineVertexInputStateCreateInfo() {
			VkPipelineVertexInputStateCreateInfo createInfo = {};
			createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
			return createInfo;
		}


		inline VULKAN_WRAPER_API VkPipelineInputAssemblyStateCreateInfo pipelineInputAssemblyStateCreateInfo() {
			VkPipelineInputAssemblyStateCreateInfo createInfo = {};
			createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
			return createInfo;
		}


		inline VULKAN_WRAPER_API VkPipelineTessellationStateCreateInfo pipelineTessellationStateCreateInfo() {
			VkPipelineTessellationStateCreateInfo createInfo = {};
			createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO;
			return createInfo;
		}


		inline VULKAN_WRAPER_API VkViewport viewport(VkExtent2D size, VkExtent2D pos = { 0,0 }) {
			VkViewport viewport = {};
			viewport.x = static_cast<float>(pos.width);
			viewport.y = static_cast<float>(pos.height);
			viewport.width = static_cast<float>(size.width);
			viewport.height = static_cast<float>(size.height);
			viewport.minDepth = 0.0f;
			viewport.maxDepth = 1.0f;
			return viewport;
		}


		inline VULKAN_WRAPER_API VkPipelineViewportStateCreateInfo pipelineViewportStateCreateInfo() {
			VkPipelineViewportStateCreateInfo createInfo = {};
			createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
			return createInfo;
		}


		inline VULKAN_WRAPER_API VkPipelineRasterizationStateCreateInfo pipelineRasterizationStateCreateInfo() {
			VkPipelineRasterizationStateCreateInfo createInfo = {};
			createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
			return createInfo;
		}


		inline VULKAN_WRAPER_API VkPipelineMultisampleStateCreateInfo pipelineMultisampleStateCreateInfo() {
			VkPipelineMultisampleStateCreateInfo createInfo = {};
			createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
			return createInfo;
		}


		inline VULKAN_WRAPER_API VkPipelineDepthStencilStateCreateInfo pipelineDepthStencilStateCreateInfo() {
			VkPipelineDepthStencilStateCreateInfo createInfo = {};
			createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
			return createInfo;
		}


		inline VULKAN_WRAPER_API VkPipelineColorBlendStateCreateInfo pipelineColorBlendStateCreateInfo() {
			VkPipelineColorBlendStateCreateInfo createInfo = {};
			createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
			return createInfo;
		}


		inline VULKAN_WRAPER_API VkPipelineDynamicStateCreateInfo pipelineDynamicStateCreateInfo() {
			VkPipelineDynamicStateCreateInfo createInfo = {};
			createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
			return createInfo;
		}


		inline VULKAN_WRAPER_API VkGraphicsPipelineCreateInfo graphicsPipelineCreateInfo() {
			VkGraphicsPipelineCreateInfo createInfo = {};
			createInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
			return createInfo;
		}


		inline VULKAN_WRAPER_API VkComputePipelineCreateInfo computePipelineCreateInfo() {
			VkComputePipelineCreateInfo createInfo = {};
			createInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
			return createInfo;
		}


		inline VULKAN_WRAPER_API VkDescriptorPoolCreateInfo descriptorPoolCreateInfo() {
			VkDescriptorPoolCreateInfo createInfo = {};
			createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
			return createInfo;
		}


		inline VULKAN_WRAPER_API VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo() {
			VkDescriptorSetLayoutCreateInfo descriptorLayoutInfo = {};
			descriptorLayoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
			return descriptorLayoutInfo;
		}


		inline VULKAN_WRAPER_API VkDescriptorSetAllocateInfo descriptorSetAllocateInfo() {
			VkDescriptorSetAllocateInfo createInfo = {};
			createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
			return createInfo;
		}


		inline VULKAN_WRAPER_API VkWriteDescriptorSet writeDescriptorSet() {
			VkWriteDescriptorSet writeInfo = {};
			writeInfo.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			return writeInfo;
		}


		inline VULKAN_WRAPER_API VkCopyDescriptorSet copyDescriptorSet() {
			VkCopyDescriptorSet copyInfo = {};
			copyInfo.sType = VK_STRUCTURE_TYPE_COPY_DESCRIPTOR_SET;
			return copyInfo;
		}


		inline VULKAN_WRAPER_API VkMemoryAllocateInfo memoryAllocateInfo() {
			VkMemoryAllocateInfo allocInfo = {};
			allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
			return allocInfo;
		}


		inline VULKAN_WRAPER_API VkBufferCreateInfo bufferCreateInfo() {
			VkBufferCreateInfo createInfo = {};
			createInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
			return createInfo;
		}
		

		inline VULKAN_WRAPER_API VkImageCreateInfo imageCreateInfo() {
			VkImageCreateInfo createInfo = {};
			createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
			return createInfo;
		}


		inline VULKAN_WRAPER_API VkImageMemoryBarrier imageMemoryBarrier() {
			VkImageMemoryBarrier imageBarrier = {};
			imageBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
			return imageBarrier;
		}


		inline VULKAN_WRAPER_API VkSamplerCreateInfo samplerCreateInfo() {
			VkSamplerCreateInfo createInfo = {};
			createInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
			return createInfo;
		}


		inline VULKAN_WRAPER_API VkFramebufferCreateInfo framebufferCreateInfo() {
			VkFramebufferCreateInfo framebufferCreateInfo{};
			framebufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
			return framebufferCreateInfo;
		}


		inline VULKAN_WRAPER_API VkCommandPoolCreateInfo commandPoolCreateInfo() {
			VkCommandPoolCreateInfo createInfo = {};
			createInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
			return createInfo;
		}

		inline VULKAN_WRAPER_API VkCommandBufferAllocateInfo commandBufferAllocateInfo() {
			VkCommandBufferAllocateInfo allocateInfo = {};
			allocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
			return allocateInfo;
		}


		inline VULKAN_WRAPER_API VkCommandBufferBeginInfo commandBufferBeginInfo() {
			VkCommandBufferBeginInfo beginnInfo = {};
			beginnInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
			return beginnInfo;
		}


		inline VULKAN_WRAPER_API VkSubmitInfo submitInfo() {
			VkSubmitInfo submitInfo = {};
			submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
			return submitInfo;
		}


		inline VULKAN_WRAPER_API VkRenderPassBeginInfo renderPassBeginInfo() {
			VkRenderPassBeginInfo renderPassInfo = {};
			renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
			return renderPassInfo;
		}
	}
}