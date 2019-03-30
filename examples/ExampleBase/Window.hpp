#pragma once
#include<vulkan_wrapper.h>
#include<glfw/glfw3.h>

namespace example {
	class Window : public vkw::Window {
	public:
		using vkw::Window::Window;
		virtual void getWindowExtensions(std::vector<const char*> & ext) = 0;
	};


	class GlfwWindow : public Window {
	public:
		GlfwWindow(int x, int y) : Window(*this)
		{
			glfwInit();
			glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
			glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
			window = glfwCreateWindow(x, y, "Vulkan", nullptr, nullptr);
		}

		~GlfwWindow() override {
			glfwDestroyWindow(static_cast<GLFWwindow*>(window));
			glfwTerminate();
		};

		void createSurface(VkInstance instance, VkSurfaceKHR * surface) const override {
			vkw::Debug::errorCodeCheck(glfwCreateWindowSurface(instance, (GLFWwindow*)window, nullptr, surface), "Failed to create Surface");
		};

		void getWindowExtensions(std::vector<const char*> & ext) override{
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
}