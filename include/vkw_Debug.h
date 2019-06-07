#pragma once
#include "vkw_Config.h"

#if VKW_DEBUG >= 1
#	define VKW_PRINT(expr) do {std::cout << (expr) << std::endl;} while(0)
#	define VKW_LOG(expr) do {std::cout << (expr) << " at line " << __LINE__ << " in file" << __FILE__ << std::endl;} while(0)
#   define VKW_assert(Expr, Msg) vkw_Assert__(#Expr, Expr, __FILE__, __LINE__, Msg)

static void vkw_Assert__(const char* expr_str, bool expr, const char* file, int line, const char* msg)
{
	if (!expr)
	{
		std::cerr << "Assert failed:\t" << msg << "\n"
			<< "Expected:\t" << expr_str << "\n"
			<< "Source:\t\t" << file << ", line " << line << "\n";
		throw std::runtime_error(0);
	}
}
#else
#	define VKW_PRINT(expr)
#	define VKW_LOG(expr)
#	define VKW_assert(Expr, Msg)
#endif


namespace vkw {
	namespace Debug {
		VULKAN_WRAPPER_API inline void errorCodeCheck(VkResult result, const char * msg = "");
	}


	namespace DebugInformationPrint {
		void printGPUInformation(VkPhysicalDevice gpu, int score = -1);
		void printPhysicalDeviceProperties(VkPhysicalDeviceProperties deviceProperties);
		void printQueueFamilyProperties(std::vector<VkQueueFamilyProperties> queueFamilies);

		void printSystemInformation(VkInstance instance);

		void printDeviceInformation();
	}
}