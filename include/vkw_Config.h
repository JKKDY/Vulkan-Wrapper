#pragma once

#define VKW_DEBUG 1
#define VKW_PRINT_STATS 1
  

#ifdef VKW_PRINT_STATS
	#include <iostream>
	#include <string>
#endif 


#if VKW_DEBUG >= 1
	#include <iostream>
	#include <sstream>
	#include <cstdlib>
#endif


#ifdef _WIN32
	#define VK_USE_PLATFORM_WIN32_KHR 1

	#ifdef VULKAN_WRAPER_EXPORT  
		#define VULKAN_WRAPER_API __declspec(dllexport)   
	#else  
		#define VULKAN_WRAPER_API __declspec(dllimport)   
	#endif
#elif defined( __linux )
	#define VK_USE_PLATFORM_XCB_KHR 1
#else
	#error Platform not yet supported
#endif 



#define NOMINMAX
#include <vulkan/vulkan.h>
#include <fstream>
#include <functional>
#include <vector>
#include <assert.h>
#include <map>
#include <unordered_map>
#include <set>
#include <array>
#include <algorithm>
#include <initializer_list>
