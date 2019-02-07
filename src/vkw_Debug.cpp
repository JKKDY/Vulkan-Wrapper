#include "vkw_Debug.h"
#include "vkw_initializers.h"

#if VKW_DEBUG >= 1


namespace vkw {
	namespace Debug {
		void errorCodeCheck(VkResult result, const char * msg)
		{
			if (result != 0) {
				std::cout << "Error: ";
				switch (result) {
				case VK_ERROR_OUT_OF_HOST_MEMORY:
					std::cout << "VK_ERROR_OUT_OF_HOST_MEMORY" << std::endl;
					break;
				case VK_ERROR_OUT_OF_DEVICE_MEMORY:
					std::cout << "VK_ERROR_OUT_OF_DEVICE_MEMORY" << std::endl;
					break;
				case VK_ERROR_INITIALIZATION_FAILED:
					std::cout << "VK_ERROR_INITIALIZATION_FAILED" << std::endl;
					break;
				case VK_ERROR_DEVICE_LOST:
					std::cout << "VK_ERROR_DEVICE_LOST" << std::endl;
					break;
				case VK_ERROR_MEMORY_MAP_FAILED:
					std::cout << "VK_ERROR_MEMORY_MAP_FAILED" << std::endl;
					break;
				case VK_ERROR_LAYER_NOT_PRESENT:
					std::cout << "VK_ERROR_LAYER_NOT_PRESENT" << std::endl;
					break;
				case VK_ERROR_EXTENSION_NOT_PRESENT:
					std::cout << "VK_ERROR_EXTENSION_NOT_PRESENT" << std::endl;
					break;
				case VK_ERROR_FEATURE_NOT_PRESENT:
					std::cout << "VK_ERROR_FEATURE_NOT_PRESENT" << std::endl;
					break;
				case VK_ERROR_INCOMPATIBLE_DRIVER:
					std::cout << "VK_ERROR_INCOMPATIBLE_DRIVER" << std::endl;
					break;
				case VK_ERROR_TOO_MANY_OBJECTS:
					std::cout << "VK_ERROR_TOO_MANY_OBJECTS" << std::endl;
					break;
				case VK_ERROR_FORMAT_NOT_SUPPORTED:
					std::cout << "VK_ERROR_FORMAT_NOT_SUPPORTED" << std::endl;
					break;
				case VK_ERROR_SURFACE_LOST_KHR:
					std::cout << "VK_ERROR_SURFACE_LOST_KHR" << std::endl;
					break;
				case VK_ERROR_NATIVE_WINDOW_IN_USE_KHR:
					std::cout << "VK_ERROR_NATIVE_WINDOW_IN_USE_KHR" << std::endl;
					break;
				case VK_SUBOPTIMAL_KHR:
					std::cout << "VK_SUBOPTIMAL_KHR" << std::endl;
					break;
				case VK_ERROR_OUT_OF_DATE_KHR:
					std::cout << "VK_ERROR_OUT_OF_DATE_KHR" << std::endl;
					break;
				case VK_ERROR_INCOMPATIBLE_DISPLAY_KHR:
					std::cout << "VK_ERROR_INCOMPATIBLE_DISPLAY_KHR" << std::endl;
					break;
				case VK_ERROR_VALIDATION_FAILED_EXT:
					std::cout << "VK_ERROR_VALIDATION_FAILED_EXT" << std::endl;
					break;
				default:
					break;
				}
				std::cout << "From vkErrorCheck:  " << msg << std::endl;
				assert(0 && "Vulkan runtime error.");
			}
		}
	}
}




#else
namespace vkw {
	namespace Debug {
		void setupDebugInstanceExtensions(std::vector<const char*>& extensions) {}

		void setupDebugInstanceLayers(std::vector<const char*>& layers) {}

		inline void errorCodeCheck(VkResult result, const char * msg) {}

		void setupDebug(VkInstance instance) {}

		void destroyDebug(VkInstance instance) {}
	}
}
#endif // DEBUG >= 1







namespace vkw{

	namespace DebugInformationPrint{


#ifdef VKW_VERBOSE

		std::string physicalDeviceTypeString(VkPhysicalDeviceType type)
		{
			switch (type)
			{
#define STR(r) case VK_PHYSICAL_DEVICE_TYPE_ ##r: return #r
				STR(OTHER);
				STR(INTEGRATED_GPU);
				STR(DISCRETE_GPU);
				STR(VIRTUAL_GPU);
#undef STR
			default: return "UNKNOWN_DEVICE_TYPE";
			}
		}

		std::string queueFlagsString(VkQueueFlags flags)
		{
			std::string flag_str;
			if (flags & VK_QUEUE_GRAPHICS_BIT) {
				flag_str += " VK_QUEUE_GRAPHICS_BIT |";
			}
			if (flags & VK_QUEUE_COMPUTE_BIT) {
				flag_str += " VK_QUEUE_COMPUTE_BIT |";
			}
			if (flags & VK_QUEUE_TRANSFER_BIT ) {
				flag_str += " VK_QUEUE_TRANSFER_BIT |";
			}
			flag_str.pop_back();
			return flag_str;
		}



		void printSystemInformation(VkInstance instance)
		{
			uint32_t layerCount = 0;
			vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
			std::vector<VkLayerProperties> layerProperties(layerCount);
			vkEnumerateInstanceLayerProperties(&layerCount, nullptr);


			uint32_t deviceCount;
			vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);
			std::vector<VkPhysicalDevice> GPUS(deviceCount);
			vkEnumeratePhysicalDevices(instance, &deviceCount, GPUS.data());


		}

		void printGPUInformation(VkPhysicalDevice gpu, int score)
		{
			bool printdeviceProperties = true;
			bool printDeviceQueueFamilyProperties = true;
			bool printdeviceFeatures = false;

			VkPhysicalDeviceProperties deviceProperties;
			vkGetPhysicalDeviceProperties(gpu, &deviceProperties);
			std::cout << "\n\n GPU " << deviceProperties.deviceName << std::endl;

			if (score > -1) {
				std::cout << "Score: " << score << "\n" << std::endl;
			}

			if (printdeviceProperties == true) {
				std::cout << "\tPhysical Device Properties: " << std::endl;
				printPhysicalDeviceProperties(deviceProperties);
			}

			if (printDeviceQueueFamilyProperties) {
				std::cout << "\tDevice Queue Family Properties: " << std::endl;
				uint32_t queueFamCount;
				vkGetPhysicalDeviceQueueFamilyProperties(gpu, &queueFamCount, nullptr);
				std::vector<VkQueueFamilyProperties> queueFamilies(queueFamCount);
				vkGetPhysicalDeviceQueueFamilyProperties(gpu, &queueFamCount, queueFamilies.data());

				printQueueFamilyProperties(queueFamilies);
			}

			if (printdeviceFeatures) {
			}
		}

		void printPhysicalDeviceProperties(VkPhysicalDeviceProperties deviceProperties)
		{
			std::cout << "\t Properties: " << std::endl;

			std::cout << "\t	apiVersion		:" << deviceProperties.apiVersion << std::endl;
			std::cout << "\t	driverVersion		:" << deviceProperties.driverVersion << std::endl;
			std::cout << "\t	vendorID		:" << deviceProperties.vendorID << std::endl;
			std::cout << "\t	deviceID		:" << deviceProperties.deviceID << std::endl;
			std::cout << "\t	deviceType		:" << physicalDeviceTypeString(deviceProperties.deviceType) << std::endl;
			std::cout << "\t	deviceName		:" << deviceProperties.deviceName << std::endl;
		}

		void printQueueFamilyProperties(std::vector<VkQueueFamilyProperties> queueFamilies)
		{
			for (uint32_t i = 0; i < queueFamilies.size(); i++) {
				std::cout << "\t QueueFamily	" << i<< std::endl;
				std::cout << "\t	 queueFlags:		" << queueFlagsString(queueFamilies[i].queueFlags) << std::endl;
				std::cout << "\t	 queueCount:		" << queueFamilies[i].queueCount << std::endl;
			}
		}

#else
		inline void printSystemInformation(VkInstance instance){}
		inline void printGPUInformation(VkPhysicalDevice gpu){}
		inline void printPhysicalDeviceProperties(VkPhysicalDeviceProperties deviceProperties){}
		inline void printDeviceQueueFamilyProperties(std::vector<VkQueueFamilyProperties> queueFamilies){}
#endif 

	}
}