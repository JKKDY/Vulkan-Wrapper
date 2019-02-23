#pragma once
#include "vkw_Config.h"

namespace vkw {
	namespace init {
		inline VkDebugUtilsMessengerCreateInfoEXT  debugUtilsMessengerCreateInfoEXT() {
			VkDebugUtilsMessengerCreateInfoEXT createInfo = {};
			createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
			return createInfo;
		}


		inline VkApplicationInfo applicationInfo() {
			VkApplicationInfo appInfo = {};
			appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
			return appInfo;
		}


		inline VkInstanceCreateInfo instanceCreateInfo() {
			VkInstanceCreateInfo createInfo = {};
			createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
			return createInfo;
		}


		inline VkDeviceQueueCreateInfo deviceQueueCreateInfo() {
			VkDeviceQueueCreateInfo createInfo = {};
			createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			return createInfo;
		}


		inline VkDeviceCreateInfo deviceCreateInfo() {
			VkDeviceCreateInfo createInfo = {}; 
			createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
			return createInfo;
		}


		inline VkSwapchainCreateInfoKHR swapchainCreateInfoKHR() {
			VkSwapchainCreateInfoKHR createInfo = {}; 
			createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
			return createInfo;
		}


		inline VkImageViewCreateInfo imageViewCreateInfo() {
			VkImageViewCreateInfo createInfo = {};
			createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			return createInfo;
		}


		inline VkComponentMapping componentMappingSwizzleIdentity() {
			VkComponentMapping mapping = {
				VK_COMPONENT_SWIZZLE_IDENTITY,
				VK_COMPONENT_SWIZZLE_IDENTITY,
				VK_COMPONENT_SWIZZLE_IDENTITY,
				VK_COMPONENT_SWIZZLE_IDENTITY
			};
			return mapping;
		}


		inline VkPresentInfoKHR presentInfoKHR() {
			VkPresentInfoKHR presentInfo = {};
			presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
			return presentInfo;
		}


		inline VkSemaphoreCreateInfo semaphoreCreateInfo() {
			VkSemaphoreCreateInfo createInfo = {};
			createInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
			return createInfo;
		}


		inline VkFenceCreateInfo fenceCreateInfo() {
			VkFenceCreateInfo createInfo = {};
			createInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
			return createInfo;
		}


		inline VkRenderPassCreateInfo renderPassCreateInfo() {
			VkRenderPassCreateInfo createInfo = {};
			createInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
			return createInfo;
		}


		inline VkShaderModuleCreateInfo shaderModuleCreateInfo() {
			VkShaderModuleCreateInfo createInfo = {};
			createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
			return createInfo;
		}


		inline VkPipelineShaderStageCreateInfo pipelineShaderStageCreateInfo() {
			VkPipelineShaderStageCreateInfo shaderStageInfo = {};
			shaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
			shaderStageInfo.pName = "main";
			return shaderStageInfo;
		}


		inline VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo() {
			VkPipelineLayoutCreateInfo createInfo = {};
			createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
			return createInfo;
		}


		inline VkPipelineCacheCreateInfo pipelineCacheCreateInfo() {
			VkPipelineCacheCreateInfo createInfo = {};
			createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;
			return createInfo;
		}


		inline VkPipelineVertexInputStateCreateInfo pipelineVertexInputStateCreateInfo() {
			VkPipelineVertexInputStateCreateInfo createInfo = {};
			createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
			return createInfo;
		}


		inline VkPipelineInputAssemblyStateCreateInfo pipelineInputAssemblyStateCreateInfo() {
			VkPipelineInputAssemblyStateCreateInfo createInfo = {};
			createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
			return createInfo;
		}


		inline VkPipelineTessellationStateCreateInfo pipelineTessellationStateCreateInfo() {
			VkPipelineTessellationStateCreateInfo createInfo = {};
			createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO;
			return createInfo;
		}


		inline VkViewport viewport(VkExtent2D size, VkExtent2D pos = { 0,0 }) {
			VkViewport viewport = {};
			viewport.x = static_cast<float>(pos.width);
			viewport.y = static_cast<float>(pos.height);
			viewport.width = static_cast<float>(size.width);
			viewport.height = static_cast<float>(size.height);
			viewport.minDepth = 0.0f;
			viewport.maxDepth = 1.0f;
			return viewport;
		}


		inline VkPipelineViewportStateCreateInfo pipelineViewportStateCreateInfo() {
			VkPipelineViewportStateCreateInfo createInfo = {};
			createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
			return createInfo;
		}


		inline VkPipelineRasterizationStateCreateInfo pipelineRasterizationStateCreateInfo() {
			VkPipelineRasterizationStateCreateInfo createInfo = {};
			createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
			createInfo.depthClampEnable = VK_FALSE;
			createInfo.lineWidth = 1.0f;
			return createInfo;
		}


		inline VkPipelineMultisampleStateCreateInfo pipelineMultisampleStateCreateInfo() {
			VkPipelineMultisampleStateCreateInfo createInfo = {};
			createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
			return createInfo;
		}


		inline VkPipelineDepthStencilStateCreateInfo pipelineDepthStencilStateCreateInfo() {
			VkPipelineDepthStencilStateCreateInfo createInfo = {};
			createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
			createInfo.front = createInfo.back;  // NOTE: from Sasha willems, look it up
			createInfo.back.compareOp = VK_COMPARE_OP_ALWAYS;
			return createInfo;
		}


		inline VkPipelineColorBlendAttachmentState pipelineColorBlendAttachmentState(VkColorComponentFlags colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT, VkBool32 blendEnable = VK_FALSE) {
			VkPipelineColorBlendAttachmentState colorBlendAttachement = {};
			colorBlendAttachement.colorWriteMask = colorWriteMask;
			colorBlendAttachement.blendEnable = blendEnable;
			return colorBlendAttachement;
		}


		inline VkPipelineColorBlendStateCreateInfo pipelineColorBlendStateCreateInfo() {
			VkPipelineColorBlendStateCreateInfo createInfo = {};
			createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
			return createInfo;
		}


		inline VkPipelineDynamicStateCreateInfo pipelineDynamicStateCreateInfo() {
			VkPipelineDynamicStateCreateInfo createInfo = {};
			createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
			return createInfo;
		}


		inline VkGraphicsPipelineCreateInfo graphicsPipelineCreateInfo() {
			VkGraphicsPipelineCreateInfo createInfo = {};
			createInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
			createInfo.basePipelineHandle = VK_NULL_HANDLE;
			createInfo.basePipelineIndex = -1;
			return createInfo;
		}


		inline VkComputePipelineCreateInfo computePipelineCreateInfo() {
			VkComputePipelineCreateInfo createInfo = {};
			createInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
			return createInfo;
		}


		inline VkDescriptorPoolCreateInfo descriptorPoolCreateInfo() {
			VkDescriptorPoolCreateInfo createInfo = {};
			createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
			return createInfo;
		}


		inline VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo() {
			VkDescriptorSetLayoutCreateInfo descriptorLayoutInfo = {};
			descriptorLayoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
			return descriptorLayoutInfo;
		}


		inline VkDescriptorSetAllocateInfo descriptorSetAllocateInfo() {
			VkDescriptorSetAllocateInfo createInfo = {};
			createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
			return createInfo;
		}


		inline VkWriteDescriptorSet writeDescriptorSet() {
			VkWriteDescriptorSet writeInfo = {};
			writeInfo.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			return writeInfo;
		}


		inline VkCopyDescriptorSet copyDescriptorSet() {
			VkCopyDescriptorSet copyInfo = {};
			copyInfo.sType = VK_STRUCTURE_TYPE_COPY_DESCRIPTOR_SET;
			return copyInfo;
		}


		inline VkMemoryAllocateInfo memoryAllocateInfo() {
			VkMemoryAllocateInfo allocInfo = {};
			allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
			return allocInfo;
		}


		inline VkBufferCreateInfo bufferCreateInfo() {
			VkBufferCreateInfo createInfo = {};
			createInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
			return createInfo;
		}

		inline VkMappedMemoryRange mappedMemoryRange() {
			VkMappedMemoryRange mappedRange = {};
			mappedRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
			return mappedRange;
		}
		

		inline VkImageCreateInfo imageCreateInfo() {
			VkImageCreateInfo createInfo = {};
			createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
			return createInfo;
		}


		inline VkImageMemoryBarrier imageMemoryBarrier() {
			VkImageMemoryBarrier imageBarrier = {};
			imageBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
			return imageBarrier;
		}


		inline VkSamplerCreateInfo samplerCreateInfo() {
			VkSamplerCreateInfo createInfo = {};
			createInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
			return createInfo;
		}


		inline VkFramebufferCreateInfo framebufferCreateInfo() {
			VkFramebufferCreateInfo framebufferCreateInfo{};
			framebufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
			return framebufferCreateInfo;
		}


		inline VkCommandPoolCreateInfo commandPoolCreateInfo() {
			VkCommandPoolCreateInfo createInfo = {};
			createInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
			return createInfo;
		}

		inline VkCommandBufferAllocateInfo commandBufferAllocateInfo() {
			VkCommandBufferAllocateInfo allocateInfo = {};
			allocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
			return allocateInfo;
		}


		inline VkCommandBufferBeginInfo commandBufferBeginInfo() {
			VkCommandBufferBeginInfo beginnInfo = {};
			beginnInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
			return beginnInfo;
		}


		inline VkSubmitInfo submitInfo() {
			VkSubmitInfo submitInfo = {};
			submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
			return submitInfo;
		}


		inline VkRenderPassBeginInfo renderPassBeginInfo() {
			VkRenderPassBeginInfo renderPassInfo = {};
			renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
			return renderPassInfo;
		}
	}
}