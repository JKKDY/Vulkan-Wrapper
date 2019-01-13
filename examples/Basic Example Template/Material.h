#pragma once
#include "Texture.h"
#include "Mesh.h"

namespace example {
	struct Material;
	struct Model;

	struct ShaderStage {
		ShaderStage() = default;
		ShaderStage(std::string fileName, VkShaderStageFlagBits stage, VkSpecializationInfo specializationInfo = {}) :
			shaderModule(fileName),
			specializationInfo(specializationInfo),
			shaderInfo(vkw::Init::pipelineShaderStageCreateInfo(shaderModule, stage, &this->specializationInfo))
		{

		}

		vkw::ShaderModule shaderModule;
		VkPipelineShaderStageCreateInfo shaderInfo;

		VkSpecializationInfo specializationInfo;
	};



	struct Shader {
		Shader() = default;
		Shader(ShaderStage * vert, ShaderStage * frag) : vertShaderStage(vert), fragShaderStage(frag) {}
		ShaderStage * vertShaderStage;
		ShaderStage * fragShaderStage;
	};


	struct MaterialLayout {
		MaterialLayout(Shader & shader, VkRenderPass renderPass, std::vector<VkDescriptorSetLayoutBinding> layoutBindings = {}) :
			renderPass(renderPass),
			shader(&shader),
			descriptorLayout(layoutBindings)
		{}

		void createPipeline(std::vector<VkDescriptorSetLayout> descrSetLayouts, std::vector<VkPushConstantRange> pushConstRanges) {
			descriptorSetLayouts = descrSetLayouts;
			pushConstantRanges = pushConstRanges;

			pipelineLayout.descriptorSetLayouts.reserve(descriptorSetLayouts.size() + 1);
			for (const VkDescriptorSetLayout x : descriptorSetLayouts) {
				pipelineLayout.descriptorSetLayouts.push_back(x);
			}

			pipelineLayout.descriptorSetLayouts.push_back(descriptorLayout);
			pipelineLayout.pushConstantRanges = pushConstantRanges;
			pipelineLayout.createPipelineLayout();

			// Vertex Input
			VkVertexInputBindingDescription vertexBindingDescription = { 0, sizeof(Vertex), VK_VERTEX_INPUT_RATE_VERTEX };

			VkVertexInputAttributeDescription positionAttrib = { 0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, Vertex::pos) };
			VkVertexInputAttributeDescription colorAttrib = { 1, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, Vertex::col) };
			VkVertexInputAttributeDescription texCoordAttrib = { 2, 0, VK_FORMAT_R32G32_SFLOAT,  offsetof(Vertex, Vertex::tex) };
			VkVertexInputAttributeDescription normAttrib = { 3, 0, VK_FORMAT_R32G32B32_SFLOAT,  offsetof(Vertex, Vertex::norm) };

			std::vector<VkVertexInputBindingDescription> vertexBindings = { vertexBindingDescription };
			std::vector<VkVertexInputAttributeDescription> vertexAttributes = { colorAttrib , positionAttrib, texCoordAttrib, normAttrib };

			pipelineStates.vertexInputState = vkw::Init::pipelineVertexInputStateCreateInfo(vertexAttributes, vertexBindings);


			// view port state
			pipelineStates.viewportState = vkw::Init::pipelineViewportSatetCreateInfo();
			pipelineStates.viewportState.scissorCount = 1;
			pipelineStates.viewportState.viewportCount = 1;


			// color blend state
			VkPipelineColorBlendAttachmentState colorBlendAttachement = vkw::Init::pipelineColorBlendAttachmentState();
			pipelineStates.colorBlendState = vkw::Init::pipelineColorBlendStateCreateInfo();
			pipelineStates.colorBlendState.attachmentCount = 1;
			pipelineStates.colorBlendState.pAttachments = &colorBlendAttachement;


			// depth stencil state
			pipelineStates.depthStencilState = vkw::Init::pipelineDepthStencilStateCreateInfo();
			pipelineStates.depthStencilState.depthTestEnable = VK_TRUE;
			pipelineStates.depthStencilState.depthWriteEnable = VK_TRUE;
			pipelineStates.depthStencilState.depthCompareOp = VK_COMPARE_OP_LESS;
			pipelineStates.depthStencilState.depthBoundsTestEnable = VK_FALSE;
			pipelineStates.depthStencilState.stencilTestEnable = VK_FALSE;


			// other pipeline States
			VkDynamicState dynamicStates[] = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };
			pipelineStates.dynamicState = pipelineDynamicStateCreateInfo(dynamicStates, 2);
			pipelineStates.inputAssemblyState = vkw::Init::pipelineInputAssemblyStateCreateInfo();
			pipelineStates.rasterizationState = pipelineRasterizationStateCreateInfo(VK_POLYGON_MODE_FILL, VK_CULL_MODE_NONE, VK_FRONT_FACE_COUNTER_CLOCKWISE);
			pipelineStates.multisampleState = vkw::Init::pipelineMultisampleStateCreateInfo();


			pipeline.shaderStages = { shader->vertShaderStage->shaderInfo, shader->fragShaderStage->shaderInfo };
			pipeline.pipelineStatePointers.pointTo(pipelineStates);
			pipeline.layout = pipelineLayout;
			pipeline.renderPass = renderPass;
			pipeline.subpass = 0;
			pipeline.createPipeline();
		}

		vkw::DescriptorSetLayout descriptorLayout;

		std::vector<VkDescriptorSetLayout> descriptorSetLayouts;
		std::vector<VkPushConstantRange> pushConstantRanges;

		Shader * shader;

		vkw::GraphicsPipeline pipeline;
		vkw::PipelineLayout pipelineLayout;
		VkRenderPass renderPass;
		vkw::GraphicsPipeline::GraphicsPipelineStates pipelineStates;

		std::vector<Material*> materials;
	};



	struct Material {
		Material(MaterialLayout & layout, std::vector<vkw::DescriptorSet::WriteInfo> writeInfos = {}) : layout(layout), writeInfos(writeInfos)
		{
			layout.materials.push_back(this);
		}

		void createMaterial(VkDescriptorPool pool) {
			descriptorSet.layout = &layout.descriptorLayout;
			descriptorSet.descriptorPool = pool;
			descriptorSet.allocateDescriptorSet();

			descriptorSet.update(writeInfos);
		}


		MaterialLayout & layout;
		vkw::DescriptorSet descriptorSet;
		std::vector<vkw::DescriptorSet::WriteInfo> writeInfos;
		std::vector<Model*> models;
	};
}#pragma once
