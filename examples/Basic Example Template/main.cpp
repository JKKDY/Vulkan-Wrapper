#include <vulkan_wraper.h>
#include <glfw3.h>
#include <glm/glm.hpp>


#include "Window.hpp"
#include "Example.h"



std::string dataPath = "../data/";
std::string modelPath = dataPath + "Models/";
std::string texturePath = dataPath + "Textures/";
std::string shaderPath = dataPath + "Shader/" + APP_NAME;



int main() {
	GlfwWindow window = GlfwWindow(800, 600);

	Example * e = new Example();

	e->run();

	glfwTerminate();
}
