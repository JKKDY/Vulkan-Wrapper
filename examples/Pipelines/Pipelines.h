#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "ExampleBase.h"

#include "Window.hpp"
#include "VkInitializers.hpp"

std::string dataPath = "../data/";
std::string modelPath = dataPath + "Models/";
std::string shaderPath = "shader/";

using namespace example;

class PipelineExample : public ExampleBase {
public:
	PipelineExample(Window & window);
	PipelineExample
	void nextFrame() override;
private:
	void setup() override;
	void loadModels();
	void createUBO();
	void createDescriptorSets();
	void createPipelines();
	void setupRenderCommanBuffers();

	struct UBO {
		glm::mat4 model;
		glm::mat4 view;
		glm::mat4 proj;
		glm::vec3 light;
	}ubo;

	struct PipelineWithShaders {
		vkw::GraphicsPipeline pipeline;
		vkw::ShaderModule vertexShader;
		vkw::ShaderModule fragmentShader;
	};

	Mesh dragon;
	vkw::Memory uniformMemory;
	vkw::Buffer uniformBuffer;
	vkw::DescriptorPool descriptorPool;
	vkw::DescriptorSetLayout setLayout;
	vkw::DescriptorSet descriptorSet;
	vkw::PipelineLayout pipelineLayout;
	PipelineWithShaders phong;
	PipelineWithShaders wire;
	PipelineWithShaders toon;
};



