#pragma once

#include "vkw_Config.h"


namespace vkw {
	namespace init {
		inline VULKAN_WRAPPER_API VkImageSubresourceRange imageSubresourceRange(VkImageAspectFlags ImageAspectFlags) {
			VkImageSubresourceRange subresourceRange = {};
			subresourceRange.aspectMask = ImageAspectFlags;  // subrecourse range describes what the image's purpose is
			subresourceRange.baseMipLevel = 0;
			subresourceRange.levelCount = 1;
			subresourceRange.baseArrayLayer = 0;
			subresourceRange.layerCount = 1;
			return subresourceRange;
		}


		inline VULKAN_WRAPPER_API VkImageViewCreateInfo imageViewCreateInfo(VkImageSubresourceRange subresourceRange, VkImage image, VkFormat format) {
			VkImageViewCreateInfo createInfo = {};
			createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			createInfo.image = image;
			createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
			createInfo.format = format;
			createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.subresourceRange = subresourceRange;
			return createInfo;
		}


		inline VULKAN_WRAPPER_API VkImageViewCreateInfo imageViewCreateInfo() {
			VkImageViewCreateInfo createInfo = {};
			createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			return createInfo;
		}

		///+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
		inline VULKAN_WRAPPER_API VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo(std::vector<VkDescriptorSetLayout> descriptorSetLayouts, std::vector<VkPushConstantRange> pushCconstants) {
			VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
			pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
			pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(descriptorSetLayouts.size());
			pipelineLayoutInfo.pSetLayouts = descriptorSetLayouts.data();
			pipelineLayoutInfo.pushConstantRangeCount = static_cast<uint32_t>(pushCconstants.size());
			pipelineLayoutInfo.pPushConstantRanges = pushCconstants.data();

			return pipelineLayoutInfo;
		}


		inline VULKAN_WRAPPER_API VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo(VkDescriptorSetLayout * setLayouts, uint32_t setLayoutCount, VkPushConstantRange * pushConstantRange, uint32_t pushConstantRangeCount) {
			VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
			pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
			pipelineLayoutInfo.pushConstantRangeCount = pushConstantRangeCount;
			pipelineLayoutInfo.pPushConstantRanges = pushConstantRange;
			pipelineLayoutInfo.setLayoutCount = setLayoutCount;
			pipelineLayoutInfo.pSetLayouts = setLayouts;
			return pipelineLayoutInfo;
		}


		inline VULKAN_WRAPPER_API VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo() {
			VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
			pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
			return pipelineLayoutInfo;
		}


		inline VULKAN_WRAPPER_API VkDescriptorSetLayoutBinding descriptorSetLayoutBinding(VkDescriptorType type, VkShaderStageFlags stageFlags, uint32_t binding, uint32_t descriptorCount = 1, VkSampler * sampler = nullptr)
		{
			VkDescriptorSetLayoutBinding setLayoutBinding = {};
			setLayoutBinding.descriptorType = type;
			setLayoutBinding.stageFlags = stageFlags;
			setLayoutBinding.binding = binding;
			setLayoutBinding.descriptorCount = descriptorCount;
			setLayoutBinding.pImmutableSamplers = sampler;
			return setLayoutBinding;
		}	


		inline VULKAN_WRAPPER_API VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo(uint32_t bindingCount, const VkDescriptorSetLayoutBinding* pBindings) {
			VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo ={};
			descriptorSetLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
			descriptorSetLayoutCreateInfo.pBindings = pBindings;
			descriptorSetLayoutCreateInfo.bindingCount = bindingCount;
			return descriptorSetLayoutCreateInfo;
		}


		inline VULKAN_WRAPPER_API VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo() {
			VkDescriptorSetLayoutCreateInfo descriptorLayoutInfo = {};
			descriptorLayoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
			return descriptorLayoutInfo;
		}
		
		
		inline VULKAN_WRAPPER_API VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo(const std::vector<VkDescriptorSetLayoutBinding> & bindings)
		{
			VkDescriptorSetLayoutCreateInfo descriptorLayoutInfo = {};
			descriptorLayoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
			descriptorLayoutInfo.pBindings = bindings.data();
			descriptorLayoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
			return descriptorLayoutInfo;
		}


		inline VULKAN_WRAPPER_API VkPipelineShaderStageCreateInfo pipelineShaderStageCreateInfo() {
			VkPipelineShaderStageCreateInfo shaderStageInfo = {};
			shaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
			return shaderStageInfo;
		}


		inline VULKAN_WRAPPER_API VkPipelineShaderStageCreateInfo pipelineShaderStageCreateInfo(VkShaderModule module, VkShaderStageFlagBits flags, VkSpecializationInfo * specializationInfo = nullptr, const char* name = "main") {
			VkPipelineShaderStageCreateInfo shaderStageInfo = {};
			shaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
			shaderStageInfo.stage = flags;
			shaderStageInfo.module = module;
			shaderStageInfo.pName = name;
			shaderStageInfo.pSpecializationInfo = specializationInfo;
			return shaderStageInfo;
		}


		inline VULKAN_WRAPPER_API VkViewport viewport(VkExtent2D size, VkExtent2D pos = {0,0}) {
			VkViewport viewport = {};
			viewport.x = (float)pos.width;
			viewport.y = (float)pos.height;
			viewport.width = (float)size.width;
			viewport.height = (float)size.height;
			viewport.minDepth = 0.0f;
			viewport.maxDepth = 1.0f;
			return viewport;
		}



		inline VULKAN_WRAPPER_API VkPipelineViewportStateCreateInfo pipelineViewportSatetCreateInfo() {
			VkPipelineViewportStateCreateInfo viewportState = {};
			viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
			return viewportState;
		}


		inline VULKAN_WRAPPER_API VkPipelineViewportStateCreateInfo pipelineViewportSatetCreateInfo(VkViewport & viewport, VkRect2D & scissor, VkPipelineViewportStateCreateFlags flags = 0) {
			VkPipelineViewportStateCreateInfo viewportState = {};
			viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
			viewportState.flags = flags;
			viewportState.pScissors = &scissor;
			viewportState.scissorCount = 1;
			viewportState.pViewports = &viewport;
			viewportState.viewportCount = 1;
			return viewportState;
		}
		
		
		inline VULKAN_WRAPPER_API VkPipelineViewportStateCreateInfo pipelineViewportSatetCreateInfo(std::vector<VkViewport> & viewports, std::vector<VkRect2D> & scissors, VkPipelineViewportStateCreateFlags flags = 0) {
			VkPipelineViewportStateCreateInfo viewportState = {};
			viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
			viewportState.flags = flags;
			viewportState.pScissors = scissors.data();
			viewportState.scissorCount = static_cast<uint32_t>(scissors.size());
			viewportState.pViewports = viewports.data();
			viewportState.viewportCount = static_cast<uint32_t>(viewports.size());
			return viewportState;
		}


		inline VULKAN_WRAPPER_API VkVertexInputBindingDescription vertexInputBindingDescription(uint32_t binding, uint32_t stride, VkVertexInputRate inputRate)
		{
			VkVertexInputBindingDescription bindingDescription = {};
			bindingDescription.binding = binding;
			bindingDescription.stride = stride;
			bindingDescription.inputRate = inputRate;
			return bindingDescription;
		}


		inline VULKAN_WRAPPER_API VkVertexInputAttributeDescription vertexInputAttributeDescription( uint32_t binding, uint32_t location, VkFormat format, uint32_t offset) 
		{
			VkVertexInputAttributeDescription vInputAttribDescription = {};
			vInputAttribDescription.location = location;
			vInputAttribDescription.binding = binding;
			vInputAttribDescription.format = format;
			vInputAttribDescription.offset = offset;
			return vInputAttribDescription;
		}


		inline VULKAN_WRAPPER_API VkPipelineVertexInputStateCreateInfo pipelineVertexInputStateCreateInfo()
		{
			VkPipelineVertexInputStateCreateInfo pipelineVertexInputStateCreateInfo = {};
			pipelineVertexInputStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
			return pipelineVertexInputStateCreateInfo;
		}
		
		
		inline VULKAN_WRAPPER_API VkPipelineVertexInputStateCreateInfo pipelineVertexInputStateCreateInfo(std::vector<VkVertexInputAttributeDescription> & attributeDescriptions, std::vector<VkVertexInputBindingDescription> & bindingDiscriptions)
		{
			VkPipelineVertexInputStateCreateInfo pipelineVertexInputStateCreateInfo = {};
			pipelineVertexInputStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
			pipelineVertexInputStateCreateInfo.pVertexAttributeDescriptions = attributeDescriptions.data();
			pipelineVertexInputStateCreateInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
			pipelineVertexInputStateCreateInfo.pVertexBindingDescriptions = bindingDiscriptions.data();
			pipelineVertexInputStateCreateInfo.vertexBindingDescriptionCount = static_cast<uint32_t>(bindingDiscriptions.size());
		
			return pipelineVertexInputStateCreateInfo;
		}


		inline VULKAN_WRAPPER_API VkPipelineInputAssemblyStateCreateInfo pipelineInputAssemblyStateCreateInfo(VkPrimitiveTopology topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, VkBool32 primitiveRestartEnable =  VK_FALSE) {
			// input assembly: how the verticies should be assembled e.g triangle list, line strip etc
			VkPipelineInputAssemblyStateCreateInfo inputAssembly = {};  
			inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
			inputAssembly.topology = topology;
			inputAssembly.primitiveRestartEnable = primitiveRestartEnable;
			return inputAssembly;
		}



		inline VULKAN_WRAPPER_API VkPipelineRasterizationStateCreateInfo pipelineRasterizationStateCreateInfo() {
			VkPipelineRasterizationStateCreateInfo pipelineRasterizationStateCreateInfo = {};
			pipelineRasterizationStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
			return pipelineRasterizationStateCreateInfo;
		}


		inline VULKAN_WRAPPER_API VkPipelineRasterizationStateCreateInfo pipelineRasterizationStateCreateInfo(VkPolygonMode polygonMode, VkCullModeFlags cullMode, VkFrontFace frontFace, VkPipelineRasterizationStateCreateFlags flags = 0)
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


		inline VULKAN_WRAPPER_API VkPipelineMultisampleStateCreateInfo pipelineMultisampleStateCreateInfo(VkPipelineMultisampleStateCreateFlags flags = 0)
		{
			// multisampling:  combines the output of the fragment shader of multiple fragments in to the same pixel
			VkPipelineMultisampleStateCreateInfo pipelineMultisampleStateCreateInfo = {};
			pipelineMultisampleStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
			pipelineMultisampleStateCreateInfo.sampleShadingEnable = VK_FALSE;
			pipelineMultisampleStateCreateInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
			pipelineMultisampleStateCreateInfo.flags = flags;
			pipelineMultisampleStateCreateInfo.minSampleShading = 1.0f; // Optional
			pipelineMultisampleStateCreateInfo.pSampleMask = nullptr; // Optional
			pipelineMultisampleStateCreateInfo.alphaToCoverageEnable = VK_FALSE; // Optional
			pipelineMultisampleStateCreateInfo.alphaToOneEnable = VK_FALSE; // Optional
			return pipelineMultisampleStateCreateInfo;
		}


		inline VULKAN_WRAPPER_API VkPipelineColorBlendAttachmentState pipelineColorBlendAttachmentState( VkColorComponentFlags colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT)
		{
			VkPipelineColorBlendAttachmentState pipelineColorBlendAttachmentState = {};
			pipelineColorBlendAttachmentState.colorWriteMask = colorWriteMask;
			pipelineColorBlendAttachmentState.blendEnable = VK_FALSE;
			pipelineColorBlendAttachmentState.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;		// alpha blend
			pipelineColorBlendAttachmentState.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
			pipelineColorBlendAttachmentState.colorBlendOp = VK_BLEND_OP_ADD;
			pipelineColorBlendAttachmentState.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
			pipelineColorBlendAttachmentState.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
			pipelineColorBlendAttachmentState.alphaBlendOp = VK_BLEND_OP_ADD;
			return pipelineColorBlendAttachmentState;
		}


		inline VULKAN_WRAPPER_API VkPipelineColorBlendStateCreateInfo pipelineColorBlendStateCreateInfo() {
			VkPipelineColorBlendStateCreateInfo pipelineColorBlendStateCreateInfo = {};
			pipelineColorBlendStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
			return pipelineColorBlendStateCreateInfo;
		}
		
		
		inline VULKAN_WRAPPER_API VkPipelineColorBlendStateCreateInfo pipelineColorBlendStateCreateInfo( std::vector<VkPipelineColorBlendAttachmentState> & attachments)
		{
			VkPipelineColorBlendStateCreateInfo pipelineColorBlendStateCreateInfo = {};
			pipelineColorBlendStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
			pipelineColorBlendStateCreateInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
			pipelineColorBlendStateCreateInfo.pAttachments = attachments.data();
			pipelineColorBlendStateCreateInfo.logicOp = VK_LOGIC_OP_COPY; // Optional
			pipelineColorBlendStateCreateInfo.blendConstants[0] = 0.0f; // Optional
			pipelineColorBlendStateCreateInfo.blendConstants[1] = 0.0f; // Optional
			pipelineColorBlendStateCreateInfo.blendConstants[2] = 0.0f; // Optional
			pipelineColorBlendStateCreateInfo.blendConstants[3] = 0.0f; // Optional
			return pipelineColorBlendStateCreateInfo;
		}


		inline VULKAN_WRAPPER_API VkPipelineDynamicStateCreateInfo pipelineDynamicStateCreateInfo(const VkDynamicState * pDynamicStates, uint32_t dynamicStateCount, VkPipelineDynamicStateCreateFlags flags = 0)
		{
			VkPipelineDynamicStateCreateInfo pipelineDynamicStateCreateInfo = {};
			pipelineDynamicStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
			pipelineDynamicStateCreateInfo.pDynamicStates = pDynamicStates;
			pipelineDynamicStateCreateInfo.dynamicStateCount = dynamicStateCount;
			pipelineDynamicStateCreateInfo.flags = flags;
			return pipelineDynamicStateCreateInfo;
		}


		inline VULKAN_WRAPPER_API VkPipelineDynamicStateCreateInfo pipelineDynamicStateCreateInfo() {
			VkPipelineDynamicStateCreateInfo pipelineDynamicStateCreateInfo = {};
			pipelineDynamicStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
			return pipelineDynamicStateCreateInfo;
		}


		inline VULKAN_WRAPPER_API VkPipelineDepthStencilStateCreateInfo pipelineDepthStencilStateCreateInfo(){
			VkPipelineDepthStencilStateCreateInfo depthStencil = {};
			depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
			return depthStencil;
		}


		inline VULKAN_WRAPPER_API VkGraphicsPipelineCreateInfo graphicsPipelineCreateInfo(VkPipelineCreateFlags flags = 0)
		{
			VkGraphicsPipelineCreateInfo graphicsPipelineCreateInfo = {};
			graphicsPipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
			graphicsPipelineCreateInfo.flags = flags;
			graphicsPipelineCreateInfo.basePipelineIndex = -1;
			graphicsPipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE;
			return graphicsPipelineCreateInfo;
		}


		inline VULKAN_WRAPPER_API VkFramebufferCreateInfo framebufferCreateInfo(VkRenderPass renderPass, VkExtent2D extent, std::vector<VkImageView> & attachements, uint32_t layers = 1, VkFramebufferCreateFlags flags = 0){
			VkFramebufferCreateInfo framebufferCreateInfo{};
			framebufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
			framebufferCreateInfo.flags = flags;
			framebufferCreateInfo.renderPass = renderPass;

			framebufferCreateInfo.attachmentCount = static_cast<uint32_t>(attachements.size());
			framebufferCreateInfo.pAttachments = attachements.data();

			framebufferCreateInfo.width = extent.width;
			framebufferCreateInfo.height = extent.height;
			framebufferCreateInfo.layers = layers;

			return framebufferCreateInfo;
		}


		inline VULKAN_WRAPPER_API VkFramebufferCreateInfo framebufferCreateInfo() {
			VkFramebufferCreateInfo framebufferCreateInfo{};
			framebufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
			return framebufferCreateInfo;
		}


		inline VULKAN_WRAPPER_API VkMemoryAllocateInfo memoryAllocateInfo(VkDeviceSize size, uint32_t memoryType){
			VkMemoryAllocateInfo allocInfo = {};
			allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
			allocInfo.pNext = nullptr;
			allocInfo.allocationSize = size;
			allocInfo.memoryTypeIndex = memoryType;
			return allocInfo;
		}


		inline VULKAN_WRAPPER_API VkMemoryAllocateInfo memoryAllocateInfo() {
			VkMemoryAllocateInfo allocInfo = {};
			allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
			return allocInfo;
		}


		inline VULKAN_WRAPPER_API VkCommandBufferAllocateInfo commandBufferAllocateInfo(VkCommandPool commandPool, uint32_t commandBufferCount, VkCommandBufferLevel level ) {
			VkCommandBufferAllocateInfo allocInfo = {};
			allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
			allocInfo.level = level;
			allocInfo.commandPool = commandPool;
			allocInfo.commandBufferCount = commandBufferCount;
			return allocInfo;
		}


		inline VULKAN_WRAPPER_API VkCommandBufferAllocateInfo commandBufferAllocateInfo() {
			VkCommandBufferAllocateInfo allocInfo = {};
			allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
			return allocInfo;
		}

		
		inline VULKAN_WRAPPER_API VkCommandBufferBeginInfo commandBufferBeginInfo(VkCommandBufferUsageFlagBits usageFlags) {
			VkCommandBufferBeginInfo beginInfo = {};
			beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
			beginInfo.flags = usageFlags;
			return beginInfo;
		}


		inline VULKAN_WRAPPER_API VkCommandBufferBeginInfo commandBufferBeginInfo() {
			VkCommandBufferBeginInfo beginInfo = {};
			beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
			return beginInfo;
		}


		inline VULKAN_WRAPPER_API VkSubmitInfo submitInfo(uint32_t commandBufferCount, VkCommandBuffer * commandBuffer){
			VkSubmitInfo submitInfo = {};
			submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
			submitInfo.commandBufferCount = commandBufferCount;
			submitInfo.pCommandBuffers = commandBuffer;
			return submitInfo;
		}


		inline VULKAN_WRAPPER_API VkFenceCreateInfo fenceCreateInfo() {
			VkFenceCreateInfo fenceInfo = {};
			fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
			return fenceInfo;
		}


		inline VULKAN_WRAPPER_API VkSemaphoreCreateInfo semaphoreCreateInfo() {
			VkSemaphoreCreateInfo semaphoreInfo = {};
			semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
			return semaphoreInfo;
		}


		inline VULKAN_WRAPPER_API VkRenderPassCreateInfo renderPassCreateInfo() {
			VkRenderPassCreateInfo renderPassInfo = {};
			renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
			return renderPassInfo;
		}


		inline VULKAN_WRAPPER_API VkSubpassDescription subpassDescription( 
			std::vector<VkAttachmentReference> * inputAttachments = nullptr, std::vector<VkAttachmentReference> * colorAttachements = nullptr, 
			std::vector<VkAttachmentReference> * resolveAttachments = nullptr, VkAttachmentReference * depthStencilAttachment = nullptr,
			std::vector<uint32_t> * preserveAttachments = nullptr, VkPipelineBindPoint bindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS)
		{
			VkSubpassDescription subPass = {};
			subPass.pipelineBindPoint = bindPoint;
			if (inputAttachments != nullptr) {
				subPass.inputAttachmentCount = static_cast<uint32_t>(inputAttachments->size());
				subPass.pInputAttachments = inputAttachments->data();
			} else {
				subPass.inputAttachmentCount = 0;
			}
			if (colorAttachements != nullptr) {
				subPass.colorAttachmentCount = static_cast<uint32_t>(colorAttachements->size());
				subPass.pColorAttachments = colorAttachements->data();
			} else {
				subPass.colorAttachmentCount = 0;
			}
			if (resolveAttachments != nullptr) {
				subPass.pResolveAttachments = resolveAttachments->data();
			}
			if (depthStencilAttachment != nullptr) {
				subPass.pDepthStencilAttachment = depthStencilAttachment;
			}
			if (preserveAttachments != nullptr) {
				subPass.preserveAttachmentCount = static_cast<uint32_t>(preserveAttachments->size());
				subPass.pPreserveAttachments = preserveAttachments->data();
			} else {
				subPass.preserveAttachmentCount = 0;
			}
			return subPass;
		}

		inline VULKAN_WRAPPER_API VkAttachmentReference attachmentReference(uint32_t attachment, VkImageLayout layout) {
			VkAttachmentReference colorAttachmentRef = {};
			colorAttachmentRef.attachment = attachment;
			colorAttachmentRef.layout = layout;
			return colorAttachmentRef;
		}


		inline VULKAN_WRAPPER_API VkAttachmentDescription attachmentDescription(VkFormat format, VkAttachmentDescriptionFlags flags = 0) {
			VkAttachmentDescription attachement = {};
			attachement.flags = flags;
			attachement.format = format;
			return attachement;
		}


		inline VULKAN_WRAPPER_API VkSubpassDependency subpassDependency() {
			VkSubpassDependency dependency = {};
			return dependency;
		}


		inline VULKAN_WRAPPER_API VkCommandPoolCreateInfo commandPoolCreateInfo(uint32_t queueFamily, VkCommandPoolCreateFlags flags = 0) {
			VkCommandPoolCreateInfo poolInfo = {};
			poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
			poolInfo.flags = flags;
			poolInfo.queueFamilyIndex = queueFamily;
			return poolInfo;
		}


		inline VULKAN_WRAPPER_API VkCommandPoolCreateInfo commandPoolCreateInfo() {
			VkCommandPoolCreateInfo poolInfo = {};
			poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
			return poolInfo;
		}


		inline VULKAN_WRAPPER_API VkRenderPassBeginInfo renderPassBeginInfo() {
			VkRenderPassBeginInfo renderPassInfo = {};
			renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
			return renderPassInfo;
		}
		
		///+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
		inline VULKAN_WRAPPER_API VkRenderPassBeginInfo renderPassBeginInfo(VkRenderPass renderPass, VkFramebuffer frameBuffer, VkRect2D renderArea, std::vector<VkClearValue> & clearValues) {
			VkRenderPassBeginInfo renderPassInfo = {};
			renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
			renderPassInfo.renderPass = renderPass;
			renderPassInfo.framebuffer = frameBuffer;
			renderPassInfo.renderArea = renderArea;
			renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
			renderPassInfo.pClearValues = clearValues.data();

			return renderPassInfo;
		}


		inline VULKAN_WRAPPER_API VkPresentInfoKHR presentInfoKHR(uint32_t * image, std::vector<VkSwapchainKHR> * swapchains, std::vector<VkSemaphore> * semaphores = nullptr, VkResult * result = nullptr) {
			VkPresentInfoKHR presentInfo = {};
			presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
			if (semaphores != nullptr) {
				presentInfo.waitSemaphoreCount = static_cast<uint32_t>(semaphores->size());
				presentInfo.pWaitSemaphores = semaphores->data();
			} 
			presentInfo.swapchainCount = static_cast<uint32_t>(swapchains->size());
			presentInfo.pSwapchains = swapchains->data();
			presentInfo.pImageIndices = image;
			presentInfo.pResults = result;

			return presentInfo;
		}


		inline VULKAN_WRAPPER_API VkPresentInfoKHR presentInfoKHR() {
			VkPresentInfoKHR presentInfo = {};
			presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
			return presentInfo;
		}


		inline VULKAN_WRAPPER_API VkSubmitInfo submitInfo(std::vector<VkCommandBuffer> * commandBuffers, std::vector<VkSemaphore> *  waitSemaphores = nullptr, 
			std::vector<VkSemaphore> * signalSemaphores = nullptr,  VkPipelineStageFlags * waitDstStageMask = nullptr) {
			VkSubmitInfo submitinfo = {};
			submitinfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
			if (waitSemaphores != nullptr) {
				submitinfo.waitSemaphoreCount = static_cast<uint32_t>(waitSemaphores->size());
				submitinfo.pWaitSemaphores = waitSemaphores->data();
			}
			if (waitSemaphores != nullptr) {
				submitinfo.signalSemaphoreCount = static_cast<uint32_t>(signalSemaphores->size());
				submitinfo.pSignalSemaphores = signalSemaphores->data();
			}
			submitinfo.pWaitDstStageMask = waitDstStageMask;
			submitinfo.commandBufferCount = static_cast<uint32_t>(commandBuffers->size());
			submitinfo.pCommandBuffers = commandBuffers->data();
			return submitinfo;
		}


		inline VULKAN_WRAPPER_API VkSubmitInfo submitInfo() {
			VkSubmitInfo submitinfo = {};
			submitinfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
			return submitinfo;
		}
		

		inline VULKAN_WRAPPER_API VkDescriptorPoolSize descriptorPoolSize(VkDescriptorType type, uint32_t descriptorCount) {
			VkDescriptorPoolSize poolSize = {};
			poolSize.type = type;
			poolSize.descriptorCount = descriptorCount;
			return poolSize;
		}


		inline VULKAN_WRAPPER_API VkDescriptorPoolCreateInfo descriptorPoolCreateInfo( std::vector<VkDescriptorPoolSize>*  poolSizes, uint32_t maxSets, VkDescriptorPoolCreateFlags flags = 0) {
			VkDescriptorPoolCreateInfo poolInfo = {};
			poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
			poolInfo.flags = flags;
			poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes->size());
			poolInfo.pPoolSizes = poolSizes->data();
			poolInfo.maxSets = maxSets;
			return poolInfo;
		}


		inline VULKAN_WRAPPER_API VkDescriptorPoolCreateInfo descriptorPoolCreateInfo() {
			VkDescriptorPoolCreateInfo poolInfo = {};
			poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
			return poolInfo;
		}


		inline VULKAN_WRAPPER_API VkBufferCreateInfo bufferCreateInfo(VkDeviceSize size, VkBufferUsageFlags usage, VkSharingMode sharingMode = VK_SHARING_MODE_EXCLUSIVE, VkBufferCreateFlags flags = 0){
			VkBufferCreateInfo bufferInfo = {};
			bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
			bufferInfo.flags = flags;
			bufferInfo.size = size;
			bufferInfo.usage = usage;
			bufferInfo.sharingMode = sharingMode;
			return bufferInfo;
		}


		inline VULKAN_WRAPPER_API VkBufferCreateInfo bufferCreateInfo(){
			VkBufferCreateInfo bufferInfo = {};
			bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
			return bufferInfo;
		}


		inline VULKAN_WRAPPER_API VkSwapchainCreateInfoKHR swapchainCreateInfoKHR() {
			VkSwapchainCreateInfoKHR createInfo = {};
			createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
			return createInfo;
		}


		inline VULKAN_WRAPPER_API VkShaderModuleCreateInfo shaderModuleCreateInfo() {
			VkShaderModuleCreateInfo  createInfo = {};
			createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
			return createInfo;
		}


		inline VULKAN_WRAPPER_API VkDescriptorSetAllocateInfo descriptorSetAllocateInfo(std::vector<VkDescriptorSetLayout> & setLayouts, VkDescriptorPool pool) {
			VkDescriptorSetAllocateInfo allocInfo = {};
			allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
			allocInfo.descriptorPool = pool;
			allocInfo.descriptorSetCount = static_cast<uint32_t>(setLayouts.size());
			allocInfo.pSetLayouts = setLayouts.data();
			return allocInfo;
		}


		inline VULKAN_WRAPPER_API VkDescriptorSetAllocateInfo descriptorSetAllocateInfo() {
			VkDescriptorSetAllocateInfo allocInfo = {};
			allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
			return allocInfo;
		}


		inline VULKAN_WRAPPER_API VkWriteDescriptorSet writeDescriptorSet() {
			VkWriteDescriptorSet descriptorWrite = {};
			descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			return descriptorWrite;
		}

		
		inline VULKAN_WRAPPER_API VkImageCreateInfo imageCreateInfo() {
			VkImageCreateInfo createInfo = {};
			createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
			return createInfo;
		}


		inline VULKAN_WRAPPER_API VkImageMemoryBarrier imageMemoryBarrier() {
			VkImageMemoryBarrier imageBarrier = {};
			imageBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
			return imageBarrier;
		}


		inline VULKAN_WRAPPER_API VkImageSubresourceLayers imageSubresourceLayers(VkImageAspectFlags flags) {
			VkImageSubresourceLayers subResource = {};
			subResource.aspectMask = flags;
			subResource.mipLevel = 0;
			subResource.layerCount = 1;
			subResource.baseArrayLayer = 0;
			return subResource;
		}


		inline VULKAN_WRAPPER_API VkBufferImageCopy bufferImageCopy(VkExtent3D imageExtent, VkImageSubresourceLayers imageSubresource, VkOffset3D imageOffset = {0,0,0}, VkDeviceSize bufferOffset = 0) {
			VkBufferImageCopy region = {};
			region.bufferOffset = bufferOffset;
			region.bufferImageHeight = 0;
			region.bufferRowLength = 0;
			region.imageSubresource = imageSubresource;
			region.imageOffset = imageOffset;
			region.imageExtent = imageExtent;
			return region;
		}


		inline VULKAN_WRAPPER_API VkSamplerCreateInfo samplerCreateInfo() {
			VkSamplerCreateInfo createInfo = {};
			createInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
			return createInfo;
		}


		inline VULKAN_WRAPPER_API VkPipelineCacheCreateInfo pipelineCacheCreateInfo() {
			VkPipelineCacheCreateInfo createInfo = {};
			createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;
			return createInfo;
		}

		inline VULKAN_WRAPPER_API VkImageCopy imageCopy(VkExtent3D extent, VkImageSubresourceLayers srcSubresource, VkImageSubresourceLayers dstSubresource, VkOffset3D srcOffset = { 0,0,0 }, VkOffset3D dstOffset = {0,0,0}) {
			VkImageCopy region = {};
			region.extent = extent;
			region.srcOffset = srcOffset;
			region.srcSubresource = srcSubresource;
			region.dstOffset = dstOffset;
			region.dstSubresource = dstSubresource;
			return region;
		}
	}
}
