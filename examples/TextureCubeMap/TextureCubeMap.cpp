#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "ExampleBase.h"

#include "Window.hpp"
#include "VkInitializers.hpp"

using namespace vkex;

class VkwExample : public ExampleBase {
public:
	VkwExample(Window & window);
	~VkwExample();
	void nextFrame() override;
private:
	void setup() override;
	void loadAssets();

};

VkwExample::VkwExample(Window & window) : ExampleBase(window) {
	auto func = [](const vkw::PhysicalDevice& phys, const vkw::Surface& surf) {
		return phys.features.samplerAnisotropy;
	};

	auto scoreFunc = [](const vkw::PhysicalDevice& phys, const vkw::Surface& surf) {
		int score = 0;
		if (phys.features.textureCompressionBC) score += 1000;
		if (phys.features.textureCompressionASTC_LDR) score += 1000;
		if (phys.features.textureCompressionETC2) score += 1000;
		return score;
	};

	InitInfo initInfo = InitInfo();
	initInfo.rateDevicefkt.push_back(scoreFunc);
	initInfo.deviceSuitableFkt.push_back(func);
	initVulkan(initInfo);
	setup();
}

VkwExample::~VkwExample()
{
	vkDeviceWaitIdle(device);
	for (auto & x : drawCommandBuffers) x.destroyObject();
}

void VkwExample::setup() {
	loadAssets();
}

void VkwExample::loadAssets() {

}

void VkwExample::nextFrame() {
	renderFrame();
}


int main() {
	GlfwWindow window(1000, 800);

	auto example = VkwExample(window);

	while (!glfwWindowShouldClose(window)) {
		example.nextFrame();
		glfwPollEvents();
	}
}

