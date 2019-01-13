#pragma once
#include "vkw_Config.h"
#include <stdlib.h>
#include <algorithm>
#include <chrono>

namespace vkw {
	inline namespace utils {
		class Null final {};


		template <typename T, typename U = Null> class ReadOnly {
		public:
			ReadOnly() = default;
			ReadOnly(const T & val) : value(val) {};
			operator const T & () const { return this->value; }
			const T * operator -> () const { return &this->value; };
		protected:
			T & get() { return value; }
			T & operator = (const T & v) { return value = v; }
		private:
			T value;
			friend U;
		};



		template <typename T, typename U = Null> class ReadOnlyVector {
		public:
			ReadOnlyVector() = default;
			ReadOnlyVector(std::initializer_list<T> init) : vec(init) {}
			const T & operator [] (int i) const { return vec[i]; }
			operator const std::vector<T>  & () const { return vec; }
			typename std::vector<T>::iterator  begin() const { return vec.begin(); }
			typename std::vector<T>::iterator  end() const { return vec.end(); }
			typename std::vector<T>::size_type size() const { return vec.size(); }
			bool empty() const { return vec.empty(); }
		protected:
			//void push_back(const T & val) { vec.push_back(val); }
			std::vector<T> & get() { return vec; }
			std::vector<T> & operator = (const std::vector<T> & v) { return vec = v; }
		private:
			std::vector<T> vec;
			friend U;
		};


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

