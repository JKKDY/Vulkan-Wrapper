#pragma once
#include "vkw_Config.h"
#include <stdlib.h>
#include <algorithm>
#include <chrono>
#include <vector>
#include <functional>

namespace vkw {
	namespace tools {
		inline void alignedFree(void* data)
		{
#if	defined(_MSC_VER) || defined(__MINGW32__)
			_aligned_free(data);
#else 
			free(data);
#endif
		}


		inline void* alignedAlloc(size_t size, size_t alignment) {

			void *data = nullptr;
#if defined(_MSC_VER) || defined(__MINGW32__)
			data = _aligned_malloc(size, alignment);
#else
			int res = posix_memalign(&data, alignment, size);
			if (res != 0)
				data = nullptr;
#endif
			return data;
		}


		inline size_t allignementOf(size_t size, VkDeviceSize minAlligment) {
			return  static_cast<size_t>(size / minAlligment) * minAlligment + (size % minAlligment) > 0 ? minAlligment : 0;
		}


		inline int findMemoryType(const VkPhysicalDeviceMemoryProperties & memoryProperties, uint32_t memoryTypeBits, VkMemoryPropertyFlags requiredProperties, VkMemoryPropertyFlags optimalProperties = 0) {
			if (optimalProperties != 0) {
				int index = findMemoryType(memoryProperties, memoryTypeBits, optimalProperties);
				if (index != -1) return index;
			}
			for (uint32_t i = 0; i < memoryProperties.memoryTypeCount; i++) {
				if ((memoryTypeBits & (1 << i)) && (memoryProperties.memoryTypes[i].propertyFlags & requiredProperties) == requiredProperties) {
					return i;
				}
			}
			return -1;
		}

		// gets biggest depth format
		inline VkFormat getDepthFormat(const VkPhysicalDevice & physicalDevice) {
			const std::vector<VkFormat> depthFormats = {
				VK_FORMAT_D32_SFLOAT_S8_UINT,
				VK_FORMAT_D32_SFLOAT,
				VK_FORMAT_D24_UNORM_S8_UINT,
				VK_FORMAT_D16_UNORM_S8_UINT,
				VK_FORMAT_D16_UNORM
			};

			for (auto & x : depthFormats)
			{
				VkFormatProperties formatProps;
				vkGetPhysicalDeviceFormatProperties(physicalDevice, x, &formatProps);
				// Format must support depth stencil attachment for optimal tiling
				if (formatProps.optimalTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT) {
					return x;
				}
			}

			return VK_FORMAT_UNDEFINED;
		}


		inline std::string getFileExtension(const std::string & filename) {
			size_t pos = filename.find_last_of(".");
			if (pos != std::string::npos)
				return filename.substr(pos + 1);
			else
				return std::string();
		}
	}

	namespace tools {
		// takes in a function to enumerate (e.g. vkEnumerateDeviceExtensionProperties), a function to get the name and desired & missing "names"
		template<typename T> std::vector<const char*> check(std::function<VkResult(uint32_t*, T*)> enumerate, std::function<const char*(const T &)> getString, const std::vector<const char*> & desired, std::vector<const char*> * missing = nullptr) {
			std::vector<const char*> exististingDesired;

			uint32_t count = 0;
			enumerate(&count, nullptr);
			std::vector<T> prop(count);
			enumerate(&count, prop.data());  

			for (const auto & x : desired) {
				if (std::find_if(prop.begin(), prop.end(), [&](const T & t) { return (strcmp(getString(t), x) == 0); }) != prop.end()) {
					exististingDesired.push_back(x);
				}
				else if (missing) {
					missing->push_back(x);
				}
			}

			return exististingDesired;
		}

		// makes class non copyable
		class NonCopyable
		{
		protected:
			NonCopyable() {}
			~NonCopyable() {}
		private:
			NonCopyable(const NonCopyable &);
			NonCopyable& operator=(const NonCopyable &) {}
		};


		template <typename T> struct PtrContainer {
			PtrContainer() = default;
			PtrContainer(T & obj) : pointer(&obj) {}
			void operator = (T * obj) { pointer = obj; }
			void operator = (T & obj) { pointer = &obj; } 
			operator T & () const { return  *pointer; }
			operator T * () const { return  pointer; }
			T & get() const { return *pointer; }
		private:
			T * pointer;
		};


		// simple Timer
		class Timer {
		public:
			using h = std::chrono::hours;
			using m = std::chrono::minutes;
			using s = std::chrono::seconds;
			using ms = std::chrono::milliseconds;
			using us = std::chrono::microseconds;
			using ns = std::chrono::nanoseconds;

			Timer() = default;

			void start() {
				count++;
				startTime = std::chrono::high_resolution_clock::now();
			}
			auto end() {
				endTime = std::chrono::high_resolution_clock::now();
				durationTime += std::chrono::duration_cast<ns> (endTime - startTime);
				return std::chrono::duration_cast<us> (endTime - startTime);
			}

			template<typename resolution = us> auto duration() {
				resolution ret = std::chrono::duration_cast<resolution> (durationTime);
				return ret.count();
			}
			template<typename resolution = us> auto avg() { 
				return duration<resolution>() / count; 
			}

			void reset() {
				count = 0;
				durationTime = std::chrono::nanoseconds::zero();
			}
			
		private:
			uint32_t count = 0;
			std::chrono::high_resolution_clock::time_point startTime;
			std::chrono::high_resolution_clock::time_point endTime;
			ns durationTime = std::chrono::nanoseconds::zero();;
		};
	}
}

