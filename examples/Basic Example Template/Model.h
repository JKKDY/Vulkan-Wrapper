#pragma once
#include "Mesh.h"
#include "Material.h"



namespace example {
	struct Scene;

	struct Model {
		Model(Material & mat, Mesh & mesh) : material(&mat), mesh(&mesh) {
			material->models.push_back(this);
		}

		void updateModelMat(glm::mat4 mat) {
			uniformModelBuffer->write(&mat, sizeof(mat), uniformOffset, true);
		};

		vkw::Buffer * uniformModelBuffer;
		Material * material;
		Mesh * mesh;

		//vkw::SubBuffer modelMatrixBuffer;
		glm::mat4 model;

		uint32_t uniformOffset;

		bool visible = true;
	};



	struct View {
		View(VkRect2D scissor, VkViewport viewport) : viewport(viewport), scissor(scissor) {}
		View(VkExtent2D ext) : scissor({ { 0,0 }, ext }), viewport(vkw::Init::viewport(ext)) {}

		VkRect2D scissor;
		VkViewport viewport;

		struct ViewProjMat {
			glm::mat4 view;
			glm::mat4 proj;
		}viewProjMat;
	};



	struct Scene {
		Scene(vkw::PhysicalDevice & gpu, std::vector<View> views, std::vector<Model*> models, std::vector<Material*> materials, std::vector<MaterialLayout*> materialLayouts) :
			views(views),
			models(models),
			materials(materials),
			materialLayouts(materialLayouts)
		{


			VkPushConstantRange viewPushConstant = {};
			viewPushConstant.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
			viewPushConstant.offset = 0;
			viewPushConstant.size = 2 * sizeof(glm::mat4);

			VkDescriptorSetLayoutBinding uboBinding = {};
			uboBinding.binding = 0;
			uboBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
			uboBinding.descriptorCount = 1;
			uboBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

			vkw::DescriptorSetLayout dynamicUBOLayout{ { uboBinding } };

			size_t maxSets = 0;
			for (auto x : materialLayouts) {
				for (auto binding : x->descriptorLayout.layoutBindings) {
					descriptorPool.poolSizes.push_back(VkDescriptorPoolSize{ binding.second.descriptorType , x->materials.size() });
					maxSets += x->materials.size();
				}
				x->createPipeline({ dynamicUBOLayout }, { viewPushConstant });
			}

			descriptorPool.poolSizes.push_back(VkDescriptorPoolSize{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC , 1 });
			descriptorPool.maxSets = materials.size() + 1;
			descriptorPool.createDescriptorPool();


			for (auto x : materials) {
				x->createMaterial(descriptorPool);
			}

			size_t uboAlignment = gpu.properties->limits.minUniformBufferOffsetAlignment;
			uniformDynamicAlignment = (sizeof(glm::mat4) / uboAlignment) * uboAlignment + ((sizeof(glm::mat4) % uboAlignment) > 0 ? uboAlignment : 0);

			modelUniformBuffer = vkw::Buffer(VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, uniformDynamicAlignment * models.size());
			modelUniformMemory.allocateMemory({ modelUniformBuffer });

			glm::mat4  * modelMatricies = (glm::mat4*) alignedAlloc(modelUniformBuffer.size, uniformDynamicAlignment);


			for (int i = 0; i < models.size(); i++) {
				glm::mat4 * modelMat = (glm::mat4*) ((uint64_t)modelMatricies + (i * uniformDynamicAlignment));
				*modelMat = models[i]->model;
				models[i]->uniformOffset = i * uniformDynamicAlignment;
				models[i]->uniformModelBuffer = &modelUniformBuffer;
				//models[i]->modelMatrixBuffer = modelUniformBuffer.createSubBuffer();
			}


			modelUniformBuffer.write(modelMatricies, uniformDynamicAlignment * models.size(), 0, true);

			scenePipelineLayout = vkw::PipelineLayout{ { dynamicUBOLayout },{ viewPushConstant } };

			modelDescriptor = vkw::DescriptorSet{ descriptorPool, dynamicUBOLayout };

			VkDescriptorBufferInfo bufferInfo = { modelUniformBuffer, 0, VK_WHOLE_SIZE };
			vkw::DescriptorSet::WriteInfo writeInfo = {};
			writeInfo.dstBinding = 0;
			writeInfo.dstArrayElement = 0;
			writeInfo.descriptorCount = 1;
			writeInfo.pBufferInfo = &bufferInfo;

			modelDescriptor.update({ writeInfo });
		}

		~Scene() {

		}

		vkw::DescriptorPool descriptorPool{};

		std::vector<View> views;
		std::vector<Model*> models;
		std::vector<Material*> materials;
		std::vector<MaterialLayout*> materialLayouts;

		vkw::PipelineLayout scenePipelineLayout;

		vkw::Memory modelUniformMemory{ VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT };
		vkw::Buffer modelUniformBuffer;
		VkDeviceSize uniformDynamicAlignment;
		vkw::DescriptorSet modelDescriptor;

	private:

	};
}
