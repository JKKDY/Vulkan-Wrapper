#pragma once
#include "vkw_Config.h"
#include <stdlib.h>
#include <algorithm>
#include <chrono>
#include <vector>
#include <functional>

namespace vkw {
	inline namespace utils {
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



		class NonCopyable
		{
		protected:
			NonCopyable() {}
			~NonCopyable() {}
		private:
			NonCopyable(const NonCopyable &);
			NonCopyable& operator=(const NonCopyable &) {}
		};




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

