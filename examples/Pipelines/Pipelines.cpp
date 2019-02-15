#include "Pipelines.h"

#include "ExampleBase.h"
#include "Window.hpp"

std::string dataPath = "../data/";
std::string modelPath = dataPath + "Models/";
std::string shaderPath = dataPath + "Shader/";


class PipelineExample : exb::ExampleBase {
public:
	PipelineExample(vkw::Window & window) : ExampleBase(window) {
		exb::InitInfo initInfo;
		initVulkan(initInfo);
		setup();
	}



private:
	void setup() override {
		loadModels();
	}

	void loadModels() {

	}

	void setupFrameBuffers() {

	}

	void setupRenderCommanBuffers() {

	}

	vkw::Buffer vertexBuffer;
	vkw::Buffer indexBufffer;
	vkw::Memory meshMemory;
	vkw::Buffer uniformBuffer;
	vkw::Memory uniformMemory;
};

int main() {
	using namespace exb;
	GlfwWindow window(1200, 800);



	
}