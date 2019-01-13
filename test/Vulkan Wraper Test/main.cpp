#include <vulkan_wraper.h>
#include <glfw3.h>

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

struct A {
	A() = default;
	A & operator = (const A & a) {
		std::cout << "ref" << std::endl;
		j = 0;
		k = 0;
		l = 0;

		return *this;
	}

	A & operator = (A && a) {
		std::cout << "refref" << std::endl;
		j = 0;
		k = 0;
		l = 0;

		return *this;
	}

	~A() {};
	int j, k, l;
};

struct B : public A {
	B() = default;
	void operator = (const B & r) {
		A::operator=(r);
	}
	void operator = (B && r) {
		A::operator=(r);
	}
};

int main() {
	
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

	vkw::TransferCommandPool transferCommandPool;
	vkw::GraphicsCommandPool graphicsCommandPool;
	vkw::ComputeCommandPool computeCommandPool;


	B a;
	a.j = 10;
	B b;
	b = B();
	
	vkw::Timer t1;
	vkw::Timer t2;
	vkw::Timer t3;
	vkw::Timer t4;
	const uint32_t num = 1000;
	
	for (int i = 0; i < 100; i++) {
		vkw::DescriptorPool vec[num];

		t4.start();
		for (int i = 0; i < num; i++) {
			vkw::DescriptorPool p = vkw::DescriptorPool::CreateInfo{};
			vec[i] = p;
		}
		t4.end();

		vkw::DescriptorPool vec2[num];
		t1.start();
		for (int i = 0; i < num; i++) {
			vkw::DescriptorPool p;
			p = vkw::DescriptorPool(vkw::DescriptorPool::CreateInfo{});
		}
		t1.end();

		vkw::DescriptorPool vec1 [num];
		t2.start();
		for (int i = 0; i < num; i++) {
			vec1[i].createDescriptorPool(vkw::DescriptorPool::CreateInfo{});
		}
		t2.end();

		t3.start();
		for (int i = 0; i < num; i++) {
			//auto ksjdj = vkw::DescriptorPool({});
		}
		t3.end();
	}
	
	std::cout << t4.duration<vkw::Timer::ms>() << std::endl;
	std::cout << t1.duration<vkw::Timer::ms>() <<std::endl;
	std::cout << t2.duration<vkw::Timer::ms>() << std::endl;
	std::cout << t3.duration<vkw::Timer::ms>() << std::endl;




	getchar();
}