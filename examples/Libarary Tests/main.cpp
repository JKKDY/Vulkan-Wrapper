#include <iostream>
#include <vulkan/vulkan.h>
#include <vulkan_wrapper.h>
#include <glfw/glfw3.h>
#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>
#include <glm/glm.hpp>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <gli/gli.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>    
#include <assimp/scene.h>
#include <assimp/postprocess.h>


std::string dataPath = "../data/";
std::string modelPath = dataPath + "Models/";
std::string texturePath = dataPath + "Textures/";
std::string shaderPath = dataPath + "Shader/";


class GlfwWindow : public vkw::Window {
public:
	GlfwWindow(int x, int y) : vkw::Window(*this)
	{
		glfwInit();
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
		window = glfwCreateWindow(x, y, "Vulkan", nullptr, nullptr);
	}

	~GlfwWindow() override {
		glfwDestroyWindow((GLFWwindow*)window);
	};

	void createSurface(VkInstance instance, VkSurfaceKHR * surface) const override {
		vkw::Debug::errorCodeCheck(glfwCreateWindowSurface(instance, (GLFWwindow*)window, nullptr, surface), "Failed to create Surface");
	};

	static void getWindowExtensions(std::vector<const char*> & ext) {
		unsigned int glfwExtensionCount = 0;
		const char** glfwExtensions;
		glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

		for (unsigned int i = 0; i < glfwExtensionCount; i++) {
			ext.push_back(glfwExtensions[i]);
		}
	};

	void getWindowSize(int * width, int * height) const override {
		glfwGetWindowSize((GLFWwindow*)window, width, height);
	};
};


struct foo {
	~foo() {
		int i = 5;
	}
};


int main() {
	foo f;

	// glfw test
	GlfwWindow window = GlfwWindow(800, 600);


	// Vulkan Wraper test
	vkw::Instance::CreateInfo instanceCreateInfo = {};
	instanceCreateInfo.applicationName = APP_NAME;
	instanceCreateInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	GlfwWindow::getWindowExtensions(instanceCreateInfo.desiredExtensions);

	vkw::Instance instance{ instanceCreateInfo };
	vkw::PhysicalDevice physicalDevice = instance.physicalDevices[0];
	vkw::Surface surface ( window, physicalDevice );
	
	vkw::Device::CreateInfo deviceCreateInfo = {};
	deviceCreateInfo.features.multiViewport = VK_TRUE;
	deviceCreateInfo.features.samplerAnisotropy = VK_TRUE;
	deviceCreateInfo.features.fillModeNonSolid = VK_TRUE;
	deviceCreateInfo.physicalDevice = physicalDevice;
	deviceCreateInfo.surfaces = { surface };

	deviceCreateInfo.preSetQueues.compute.index = 3;
	deviceCreateInfo.preSetQueues.compute.family = 0;
	
	deviceCreateInfo.preSetQueues.graphics.family = 0;
	deviceCreateInfo.preSetQueues.graphics.index = 4;

	vkw::Device::AdditionalQueueCreateInfo adq;
	adq.family = 0;
	adq.index = 2;
	adq.priorities = { 0.5, 0.5 };
	
	vkw::Device::AdditionalQueueCreateInfo adq1;
	adq1.family = 0;
	adq1.index = 5;
	adq1.priorities = { 0.7, 0.7 };
	
	deviceCreateInfo.additionalQueues = { adq, adq1 };

	vkw::Device device{ deviceCreateInfo };
	vkw::Swapchain swapChain{ surface };
	
	vkw::TransferCommandPool transferCommandPool{};
	vkw::GraphicsCommandPool graphicsCommandPool{};
	
	vkw::Buffer buf;
	buf = vkw::Buffer();


	// tiny obj loader test
	std::string inputfile = modelPath + "Teapot.obj";
	tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;

	std::string warn;
	std::string err;
	bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, inputfile.c_str());
	if (!warn.empty()) std::cerr << warn << std::endl;
	if (!err.empty()) std::cerr << err << std::endl; 
	if (!ret) exit(1);
	std::cout << "tiny obj loader test successful" << std::endl;



	// stb test
	int texWidth, texHeight, texChannels;
	std::string path = texturePath + "picture.png";
	stbi_uc * pixels = stbi_load(path.c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
	std::cout << "stb test successful" << std::endl;



	// glm test
	glm::mat4 matrix;
	glm::vec4 vec;
	auto test = matrix * vec;
	std::cout << "glm test successful" << std::endl;


	
	// gli test
	gli::texture_cube texCube(gli::load(texturePath + "cubemap_yokohama_bc3_unorm.ktx"));
	assert(!texCube.empty());
	std::cout << "gli test successful" << std::endl;



	// assimp test
	Assimp::Importer importer;
	const aiScene * scene = importer.ReadFile(modelPath + "Teapot.obj",
		aiProcess_CalcTangentSpace |
		aiProcess_Triangulate |
		aiProcess_JoinIdenticalVertices |
		aiProcess_SortByPType);

	if (!scene) {
		std::cout << importer.GetErrorString() << std::endl;
		exit(-1);
	}
	std::cout << "assimp test successful" << std::endl;


	//getchar();

	window.~GlfwWindow();
	glfwTerminate();
}