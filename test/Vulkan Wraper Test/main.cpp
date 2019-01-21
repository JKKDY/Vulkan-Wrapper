#include <vulkan_wraper.h>
#include <glfw/glfw3.h>

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
		glfwDestroyWindow(static_cast<GLFWwindow*>(window));
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

	operator GLFWwindow * () { return static_cast<GLFWwindow*>(window); }
};

struct foo {
	~foo() {
		int i = 5;
	}
};

int main() {
	
	foo fff;

	GlfwWindow window = GlfwWindow(800, 600);


	/// Core
	vkw::Instance::CreateInfo instanceCreateInfo = {};
	instanceCreateInfo.applicationName = "Triangle";
	instanceCreateInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	GlfwWindow::getWindowExtensions(instanceCreateInfo.desiredExtensions);

	instanceCreateInfo.desiredExtensions = vkw::Instance::checkExtensions(instanceCreateInfo.desiredExtensions);

	vkw::Instance instance(instanceCreateInfo);
	vkw::PhysicalDevice physicalDevice= instance.physicalDevices[1];
	vkw::Surface surface(window, physicalDevice);

	vkw::Device::CreateInfo deviceCreateInfo = {};
	deviceCreateInfo.features.multiViewport = VK_TRUE;
	deviceCreateInfo.features.samplerAnisotropy = VK_TRUE;
	deviceCreateInfo.features.fillModeNonSolid = VK_TRUE;
	deviceCreateInfo.physicalDevice = physicalDevice;
	deviceCreateInfo.surfaces = { surface };
	
	vkw::Device device(deviceCreateInfo);
	vkw::Swapchain swapChain(surface);

	vkw::Swapchain s;
	s.createSwapchain(surface);

	vkw::Fence f;
	f.createFence();

	f.destroyObject();

	vkw::Device dev;
	dev.createDevice(deviceCreateInfo);

	f.createFence();

	vkw::TransferCommandPool transferCommandPool;
	vkw::GraphicsCommandPool graphicsCommandPool;
	vkw::ComputeCommandPool computeCommandPool;

}