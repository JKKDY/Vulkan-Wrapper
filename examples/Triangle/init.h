#pragma once
#include <vulkan/vulkan.h>
#include <vector>


inline VkPipelineViewportStateCreateInfo pipelineViewportSatetCreateInfo(VkViewport & viewport, VkRect2D & scissor, VkPipelineViewportStateCreateFlags flags = 0) {
	VkPipelineViewportStateCreateInfo viewportState = {};
	viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewportState.flags = flags;
	viewportState.pScissors = &scissor;
	viewportState.scissorCount = 1;
	viewportState.pViewports = &viewport;
	viewportState.viewportCount = 1;
	return viewportState;
}


inline VkPipelineViewportStateCreateInfo pipelineViewportSatetCreateInfo(std::vector<VkViewport> & viewports, std::vector<VkRect2D> & scissors, VkPipelineViewportStateCreateFlags flags = 0) {
	VkPipelineViewportStateCreateInfo viewportState = {};
	viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewportState.flags = flags;
	viewportState.pScissors = scissors.data();
	viewportState.scissorCount = static_cast<uint32_t>(scissors.size());
	viewportState.pViewports = viewports.data();
	viewportState.viewportCount = static_cast<uint32_t>(viewports.size());
	return viewportState;
}


inline VkPipelineRasterizationStateCreateInfo pipelineRasterizationStateCreateInfo(VkPolygonMode polygonMode, VkCullModeFlags cullMode, VkFrontFace frontFace, VkPipelineRasterizationStateCreateFlags flags = 0)
{
	// rasterizer: turns verticies into fragments to be colored by the fragment shader (also performs depth testing,  face culling and scissor test)
	VkPipelineRasterizationStateCreateInfo pipelineRasterizationStateCreateInfo = {};
	pipelineRasterizationStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	pipelineRasterizationStateCreateInfo.polygonMode = polygonMode;
	pipelineRasterizationStateCreateInfo.cullMode = cullMode;
	pipelineRasterizationStateCreateInfo.frontFace = frontFace;
	pipelineRasterizationStateCreateInfo.flags = flags;
	pipelineRasterizationStateCreateInfo.depthClampEnable = VK_FALSE;
	pipelineRasterizationStateCreateInfo.lineWidth = 1.0f;
	pipelineRasterizationStateCreateInfo.rasterizerDiscardEnable = VK_FALSE;
	pipelineRasterizationStateCreateInfo.depthBiasEnable = VK_FALSE;
	pipelineRasterizationStateCreateInfo.depthBiasConstantFactor = 0.0f; // Optional
	pipelineRasterizationStateCreateInfo.depthBiasClamp = 0.0f; // Optional
	pipelineRasterizationStateCreateInfo.depthBiasSlopeFactor = 0.0f; // Optional
	return pipelineRasterizationStateCreateInfo;
}


inline VkPipelineShaderStageCreateInfo pipelineShaderStageCreateInfo(VkShaderModule module, VkShaderStageFlagBits flags, VkSpecializationInfo * specializationInfo = nullptr, const char* name = "main") {
	VkPipelineShaderStageCreateInfo shaderStageInfo = {};
	shaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	shaderStageInfo.stage = flags;
	shaderStageInfo.module = module;
	shaderStageInfo.pName = name;
	shaderStageInfo.pSpecializationInfo = specializationInfo;
	return shaderStageInfo;
}


inline VkImageSubresourceLayers imageSubresourceLayers(VkImageAspectFlags flags) {
	VkImageSubresourceLayers subResource = {};
	subResource.aspectMask = flags;
	subResource.mipLevel = 0;
	subResource.layerCount = 1;
	subResource.baseArrayLayer = 0;
	return subResource;
}