#include <vulkan/vulkan.h>
#include <vulkan_wrapper.h>
#include <glfw/glfw3.h>
#include <glm/glm.hpp>

#include "init.h"


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

	// returns required extensions
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


const std::string shaderPath = "Shader/";


// simple debug callback
static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
	VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
	VkDebugUtilsMessageTypeFlagsEXT messageType,
	const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
	void* pUserData)
{
	std::cerr << "validation layer: " << pCallbackData->pMessage << "\n";

	return VK_FALSE;
}

int main() {
	GlfwWindow window = GlfwWindow(800, 600);

	/// Core
	VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo = vkw::init::debugUtilsMessengerCreateInfoEXT();
	debugCreateInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
	debugCreateInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
	debugCreateInfo.pfnUserCallback = debugCallback;
	debugCreateInfo.pUserData = nullptr;

	vkw::Instance::CreateInfo instanceCreateInfo = {};
	instanceCreateInfo.debugMessengerInfos = { debugCreateInfo };
	instanceCreateInfo.desiredExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
	instanceCreateInfo.desiredLayers.push_back("VK_LAYER_LUNARG_standard_validation");
	GlfwWindow::getWindowExtensions(instanceCreateInfo.desiredExtensions);

	// query if extensions exist
	std::vector<const char*> missingExtensions;
	instanceCreateInfo.desiredExtensions = vkw::Instance::checkExtensions(instanceCreateInfo.desiredExtensions, &missingExtensions);
	VKW_assert(missingExtensions.empty(), "Required Extensions missing");

	// query if requried layers are supported
	std::vector<const char*> missingLayers;
	instanceCreateInfo.desiredLayers = vkw::Instance::checkLayers(instanceCreateInfo.desiredLayers, &missingLayers);
	VKW_assert(missingLayers.empty(), "Required Layers missing");

	vkw::Instance instance(instanceCreateInfo);
	vkw::Surface surface(window);

	VKW_assert(!instance.physicalDevices.empty(), "failed to find GPUs with Vulkan support!");
	std::vector<vkw::PhysicalDevice> suitableDevices;

	//check for suitability
	for (auto & physicalDevice : instance.physicalDevices) {
		bool familiesFound = false;

		// check for present support
		for (uint32_t i = 0; i < physicalDevice.queueFamilyProperties.size(); i++) {
			VkBool32 presentSupport = false;
			vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, i, surface, &presentSupport);
			if (presentSupport) familiesFound = true;
		}

		// check for graphics and transfer capabilities
		familiesFound = physicalDevice.queueFamilyTypes.graphicFamilies.size() * physicalDevice.queueFamilyTypes.transferFamilies.size();
		
		// check for required layers
		std::vector<const char*> missingLayers;
		physicalDevice.checkLayers({ VK_KHR_SWAPCHAIN_EXTENSION_NAME }, &missingLayers);
		
		if (familiesFound * surface.formats(physicalDevice).size() * surface.presentModes(physicalDevice).size() * missingLayers.empty()) 
			suitableDevices.push_back(physicalDevice);
	}

	//select best device
	std::map<int, vkw::PhysicalDevice*> candidates;
	for (auto &physicalDevice : suitableDevices) {
		int score = 0;

		// Discrete gpus generally perform better 
		if (physicalDevice.properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) score += 1000;
		score += physicalDevice.properties.limits.maxImageDimension2D;  //affects Image Quaity

		// dedicated transfer family are usually faster 
		for (int index : physicalDevice.queueFamilyTypes.transferFamilies) {
			if (physicalDevice.queueFamilyProperties.at(index).queueFlags == VK_QUEUE_TRANSFER_BIT) {
				score += 500;
				break;
			}
		}

		// graphics queue with present support perform generally better
		for (int index : physicalDevice.queueFamilyTypes.graphicFamilies) {
			VkBool32 presentSupport = false;
			vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, index, surface, &presentSupport);
			if (presentSupport)  score += 500;
		}

		candidates.insert(std::make_pair(score, &physicalDevice));
	}

	vkw::PhysicalDevice & physicalDevice = *candidates.rbegin()->second;

	vkw::Device::CreateInfo deviceCreateInfo = {};
	deviceCreateInfo.physicalDevice = physicalDevice;
	deviceCreateInfo.surfaces = { surface };
	deviceCreateInfo.extensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
	
	vkw::Device device(deviceCreateInfo);
	vkw::Swapchain swapChain(surface);

	// VKW_DEFAULT_QUEUE = let the implementation choose 
	vkw::TransferCommandPool transferCommandPool(VKW_DEFAULT_QUEUE); 
	vkw::GraphicsCommandPool graphicsCommandPool(VKW_DEFAULT_QUEUE);
	vkw::ComputeCommandPool computeCommandPool(VKW_DEFAULT_QUEUE);





	/// Render Pass
	// one subPass with one external dependency and one color attachment 
	VkSubpassDependency dependency = {};
	dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
	dependency.dstSubpass = 0;
	dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.srcAccessMask = 0;
	dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

	VkAttachmentDescription colorAttachment = {};
	colorAttachment.format = swapChain.surfaceFormat.format;
	colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
	colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

	VkAttachmentReference colorAttachmentRef = { 0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL };

	VkSubpassDescription subpass = {};
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.colorAttachmentCount = 1;
	subpass.pColorAttachments = &colorAttachmentRef;


	vkw::RenderPass::CreateInfo renderPassCreateInfo = {};
	renderPassCreateInfo.subPasses.push_back(subpass);
	renderPassCreateInfo.dependencys.push_back(dependency);
	renderPassCreateInfo.attachements.push_back(colorAttachment);

	vkw::RenderPass renderPass(renderPassCreateInfo);




	/// Vertex & index Buffer
	struct Vertex {
		glm::vec2 pos;
		glm::vec3 col;
	};

	// vertices and indices of the triangle
	const std::vector<Vertex> vertices = {
		{ { 0.0f, -0.5f },{ 1.0f, 0.0f, 0.0f } },
		{ { 0.5f, 0.5f },{ 0.0f, 1.0f, 0.0f } },
		{ { -0.5f, 0.5f },{ 0.0f, 0.0f, 1.0f } }
	};

	const std::vector<uint16_t> indices = { 0, 1, 2 };

	// allocating staging buffer (VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT) and vertex & index buffer (VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT)
	vkw::Memory vertexMemory(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
	vkw::Buffer vertexBuffer(VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, sizeof(Vertex) * vertices.size());
	vertexMemory.allocateMemory({vertexBuffer});

	vkw::Memory indexMemory(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
	vkw::Buffer indexBuffer(VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, sizeof(uint16_t) * indices.size());
	indexMemory.allocateMemory({indexBuffer });

	vkw::Memory vertexStagingMemory(VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
	vkw::Buffer vertexStagingBuffer(VK_BUFFER_USAGE_TRANSFER_SRC_BIT, sizeof(Vertex) * vertices.size());
	vertexStagingMemory.allocateMemory({vertexStagingBuffer });

	vkw::Memory indexStagingMemory(VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
	vkw::Buffer indexStagingBuffer(VK_BUFFER_USAGE_TRANSFER_SRC_BIT, sizeof(uint16_t) * indices.size());
	indexStagingMemory.allocateMemory({indexStagingBuffer });
	
	// using Subbuffers for data writing and transfer 
	vkw::SubBuffer subIndexStagingBuffer = indexStagingBuffer.createSubBuffer(indexStagingBuffer.size);
	vkw::SubBuffer subVertesStagingBuffer = vertexStagingBuffer.createSubBuffer(vertexStagingBuffer.size);

	vkw::SubBuffer subVertesBuffer = vertexBuffer.createSubBuffer(vertexBuffer.size);
	vkw::SubBuffer subIndexBuffer = indexBuffer.createSubBuffer(indexBuffer.size);

	// write to staging Buffers
	subIndexStagingBuffer.write(indices.data(), indices.size() * sizeof(indices[0]));
	subVertesStagingBuffer.write(vertices.data(), vertices.size() * sizeof(vertices[0]));

	// transfer to device local buffers
	subIndexBuffer.copyFrom(subIndexStagingBuffer);
	subVertesBuffer.copyFrom(subVertesStagingBuffer);




	// Shader Modules
	vkw::ShaderModule vertShaderModule(shaderPath + "shader.vert.spv", VK_SHADER_STAGE_VERTEX_BIT);    //Shader Module for vertex Shader
	vkw::ShaderModule fragShaderModule(shaderPath + "shader.frag.spv", VK_SHADER_STAGE_FRAGMENT_BIT);	// Shader Module for fragment Shader

	/// Graphics Pipeline
	// pipeline Layout without any descriptorSetLayouts and pushConstants
	vkw::PipelineLayout layout({}, {});

	// vertex input state
	VkVertexInputBindingDescription vertexInputBindingDescription = {};
	vertexInputBindingDescription.binding = 0;
	vertexInputBindingDescription.stride = sizeof(Vertex);
	vertexInputBindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

	std::array<VkVertexInputAttributeDescription, 2> vertexInputAttributs;

	vertexInputAttributs[0].binding = 0;
	vertexInputAttributs[0].location = 0;
	vertexInputAttributs[0].format = VK_FORMAT_R32G32_SFLOAT;
	vertexInputAttributs[0].offset = offsetof(Vertex, pos);

	vertexInputAttributs[1].binding = 0;
	vertexInputAttributs[1].location = 1;
	vertexInputAttributs[1].format = VK_FORMAT_R32G32B32_SFLOAT;
	vertexInputAttributs[1].offset = offsetof(Vertex, col);

	VkPipelineVertexInputStateCreateInfo vertexInputState = vkw::init::pipelineVertexInputStateCreateInfo();
	vertexInputState.pVertexAttributeDescriptions = vertexInputAttributs.data();
	vertexInputState.vertexAttributeDescriptionCount = static_cast<uint32_t>(vertexInputAttributs.size());
	vertexInputState.pVertexBindingDescriptions = &vertexInputBindingDescription;
	vertexInputState.vertexBindingDescriptionCount = 1;

	// viewport State
	VkRect2D scissor = { { 0,0 }, swapChain.extent };
	VkViewport viewport = vkw::init::viewport(swapChain.extent);

	VkPipelineViewportStateCreateInfo viewportState = pipelineViewportSatetCreateInfo(viewport, scissor);

	// collor blend State
	VkPipelineColorBlendAttachmentState colorBlendAttachement = {};
	colorBlendAttachement.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
	colorBlendAttachement.blendEnable = VK_FALSE;

	VkPipelineColorBlendStateCreateInfo colorBlendState = vkw::init::pipelineColorBlendStateCreateInfo();
	colorBlendState.attachmentCount = 1;
	colorBlendState.pAttachments = &colorBlendAttachement;

	// other pipeline States
	VkPipelineInputAssemblyStateCreateInfo inputAssemblyState = vkw::init::pipelineInputAssemblyStateCreateInfo();
	inputAssemblyState.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	inputAssemblyState.primitiveRestartEnable = VK_FALSE;

	VkPipelineRasterizationStateCreateInfo rasterizationState = pipelineRasterizationStateCreateInfo(VK_POLYGON_MODE_FILL, 0, VK_FRONT_FACE_CLOCKWISE);

	VkPipelineMultisampleStateCreateInfo multisampleState = vkw::init::pipelineMultisampleStateCreateInfo();
	multisampleState.sampleShadingEnable = VK_FALSE;
	multisampleState.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

	// Pipeline Sates
	vkw::GraphicsPipeline::CreateInfo pipelineCreateInfo;
	pipelineCreateInfo.subPass = 0;
	pipelineCreateInfo.shaderStages = { vertShaderModule.shaderStageInfo(), fragShaderModule.shaderStageInfo() };
	pipelineCreateInfo.renderPass = renderPass;
	pipelineCreateInfo.layout = layout;
	pipelineCreateInfo.vertexInputState = &vertexInputState;
	pipelineCreateInfo.viewportState = &viewportState;
	pipelineCreateInfo.colorBlendState = &colorBlendState;
	pipelineCreateInfo.inputAssemblyState = &inputAssemblyState;
	pipelineCreateInfo.rasterizationState = &rasterizationState;
	pipelineCreateInfo.multisampleState = &multisampleState;

	// Graphics Pipeline creation
	vkw::GraphicsPipeline pipeline(pipelineCreateInfo);





	/// Frame Buffers
	// Frame buffer with imageView from the swapchain as attachement
	std::vector<vkw::FrameBuffer> framebuffers(swapChain.imageCount);
	for (uint32_t i = 0; i < framebuffers.size(); i++) {
		vkw::FrameBuffer::CreateInfo createInfo = {};
		createInfo.attachments = { swapChain.imageView(i) };
		createInfo.renderPass = renderPass;
		createInfo.extent = swapChain.extent;
		framebuffers[i].createFrameBuffer(createInfo);
	}





	/// Command Buffers
	// commandbuffers for drawinf the triangle
	std::vector<vkw::CommandBuffer> commandBuffers(swapChain.imageCount);
	vkw::CommandBuffer::allocateCommandBuffers(commandBuffers, graphicsCommandPool);

	VkClearValue clearCol = { 0.0f, 0.0f, 0.0f, 1.0f };

	VkRenderPassBeginInfo beginnInfo = vkw::init::renderPassBeginInfo();
	beginnInfo.renderPass = renderPass;
	beginnInfo.renderArea = { { 0,0 }, swapChain.extent };
	beginnInfo.clearValueCount = 1;
	beginnInfo.pClearValues = &clearCol;

	for (uint32_t i = 0; i < commandBuffers.size(); i++) {
		beginnInfo.framebuffer = framebuffers[i];

		commandBuffers[i].beginCommandBuffer(VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT);

		vkCmdBeginRenderPass(commandBuffers[i], &beginnInfo, VK_SUBPASS_CONTENTS_INLINE);

			vkCmdBindPipeline(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);

			vkCmdBindIndexBuffer(commandBuffers[i], indexBuffer, 0, VK_INDEX_TYPE_UINT16);

			VkBuffer vertexBuffers[] = { vertexBuffer };
			VkDeviceSize offsets[] = { 0 };
			vkCmdBindVertexBuffers(commandBuffers[i], 0, 1, vertexBuffers, offsets);
			
			vkCmdDrawIndexed(commandBuffers[i], static_cast<uint32_t>(indices.size()), 1, 0, 0, 0);

		vkCmdEndRenderPass(commandBuffers[i]);

		commandBuffers[i].endCommandBuffer();
	}




	/// Rendering and Presenting
	vkw::Semaphore semaphore({});
	vkw::Fence fence({});

	uint32_t index = swapChain.getNextImage(VK_NULL_HANDLE, fence);
	fence.wait();
	commandBuffers[index].submitCommandBuffer(device.graphicsQueue, { semaphore });
	swapChain.presentImage(index, { semaphore });

	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();
	}

	// shutting down
	vkDeviceWaitIdle(device);

	window.~GlfwWindow();
	glfwTerminate();
}
