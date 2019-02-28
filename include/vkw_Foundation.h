#pragma once
#include "vkw_Config.h"
#include "vkw_Debug.h"
#include "vkw_Utils.h"
#include <functional>

// add optional queue paramater to all function that use queues

namespace vkw {
	struct PhysicalDevice;

	enum DestructionControl : uint32_t {
		VKW_DESTR_CONTRL_DO_NOTHING = 1,
		VKW_DESTR_CONTRL_FIRST_OBJECT_CALLS_DELETER = 2,	// first object to be deleted destroys the Vulkan object
		VKW_DESTR_CONTRL_LAST_OBJECT_CALLS_DELETER = 3,		// last object to be deleted destroy the vulkan object
		VKW_DESTR_CONTRL_EXCLUSIVE_DELETER_CALL = 4			// this object when deleted destroys the vulkan object	
	};

	namespace impl {
		class Registry;
		class RegistryManager;

		struct VkwInstance				{using Type = VkInstance			;};
		struct VkwPhysicalDevice		{using Type = VkPhysicalDevice		;};
		struct VkwDevice				{using Type = VkDevice				;};

		struct VkwQueue					{using Type = VkQueue				;};
		struct VkwSurfaceKHR			{using Type = VkSurfaceKHR			;};

		struct VkwGraphicsCommandPool	{using Type = VkCommandPool			;};
		struct VkwTransferCommandPool	{using Type = VkCommandPool			;};
		struct VkwComputeCommandPool	{using Type = VkCommandPool			;};

		struct VkwBuffer				{using Type = VkBuffer				;};
		struct VkwBufferView			{using Type = VkBufferView			;};
		struct VkwCommandBuffer			{using Type = VkCommandBuffer		;};
		struct VkwCommandPool			{using Type = VkCommandPool			;};
		struct VkwDescriptorPool		{using Type = VkDescriptorPool		;};
		struct VkwDescriptorSetLayout	{using Type = VkDescriptorSetLayout	;};
		struct VkwDescriptorSet			{using Type = VkDescriptorSet		;};
		struct VkwDeviceMemory			{using Type = VkDeviceMemory		;};
		struct VkwFence					{using Type = VkFence				;};
		struct VkwFramebuffer			{using Type = VkFramebuffer			;};
		struct VkwEvent					{using Type = VkEvent				;};
		struct VkwImage					{using Type = VkImage				;};
		struct VkwImageView				{using Type = VkImageView			;};
		struct VkwPipelineCache			{using Type = VkPipelineCache		;};
		struct VkwPipeline				{using Type = VkPipeline			;};
		struct VkwPipelineLayout		{using Type = VkPipelineLayout		;};
		struct VkwRenderPass			{using Type = VkRenderPass			;};
		struct VkwSampler				{using Type = VkSampler				;};
		struct VkwSemaphore				{using Type = VkSemaphore			;};
		struct VkwShaderModule			{using Type = VkShaderModule		;};
		struct VkwSwapchainKHR			{using Type = VkSwapchainKHR		;};
		struct VkwQueryPool				{using Type = VkQueryPool			;};



		template<typename T> class VkObject : utils::NonCopyable{
			using Type = typename T::Type;
		public:
			VkObject(std::function<void(Type)> & delf, std::function<void(void)> callback);
			~VkObject();

			void add(VkObject<T> *& ref);
			void remove(VkObject<T> *& ref, std::function<void(Type)> deleterf = 0);
			void deleteThis(std::function<void(Type)> deleterf = 0);  //delterf = overwrite to ::deleterFunc 
			uint32_t referenceCount();

			Type * getPointer();
			Type getObject();
			operator typename T::Type();

			void operator = (const Type & rhs);
		private:
			std::function<void(void)> deleterCallback;
			std::function<void(Type)> deleterFunc = 0;
			std::vector<VkObject<T>**> references;
			Type object = VK_NULL_HANDLE;
		};



		template<typename T, typename RegType> class VkPointer {
			using Type = typename T::Type;
		public:
			VkPointer(RegType & reg);
			VkPointer(const VkPointer<T, RegType> & obj);

			void copy(const VkPointer<T, RegType> & obj, DestructionControl destrContr);
			void destroyObject(DestructionControl destrContr, std::function<void(Type)> deleterf = 0);

			operator Type * () const;
			Type operator *() const;
			void operator = (Type rhs);
			//VkPointer<T, RegType> & operator = (VkPointer<T, RegType> & rhs);

		private:
			RegType & registry;
			virtual void createNewObject() const;
			mutable VkObject<T> * pObject = nullptr;
		};



		template<typename T, typename RegType> class Base {
			using Type = typename T::Type;
		public:
			Base();
			VULKAN_WRAPPER_API Base(const Base<T, RegType> & rhs);
			VULKAN_WRAPPER_API ~Base();

			DestructionControl destructionControl = VKW_DESTR_CONTRL_LAST_OBJECT_CALLS_DELETER;
			bool passOnVkObject = true;

			VULKAN_WRAPPER_API virtual void destroyObject();
			VULKAN_WRAPPER_API Base<T, RegType> & operator = (const Base<T, RegType> & rhs);
			//VULKAN_WRAPER_API Base<T, RegType> & operator = (Base<T, RegType> && rhs);
			VULKAN_WRAPPER_API operator typename T::Type () const;
			VULKAN_WRAPPER_API Type * get() const; // make const
		protected:
			RegType & registry;
			VkPointer<T, RegType> pVkObject;
		};

		template <typename T> using Entity = Base<T, Registry>;
		template <typename T> using CoreEntity = Base<T, RegistryManager>;






		struct PhysicalDevice {
			VkPhysicalDevice physicalDevice;
			std::vector<VkQueueFamilyProperties> queueFamilyProperties;
			VkPhysicalDeviceProperties properties;
			VkPhysicalDeviceFeatures features;
			VkPhysicalDeviceMemoryProperties memoryProperties;

			operator VkPhysicalDevice () const;
		};

		struct Surface {
			VkSurfaceKHR surface;
			std::vector<VkSurfaceFormatKHR> availableFomats;
			std::vector<VkPresentModeKHR> availablePresentModes;
			VkSurfaceCapabilitiesKHR capabilities;
			VkExtent2D extent;

			operator VkSurfaceKHR () const;
		};

		struct DeviceQueue {
			VkQueue queue; 
			int family;

			operator VkQueue() const;
		};


		template <typename T> struct VkReference {
			VkReference(T *& ref) : reference(ref) {}
			operator T () const { return reference ? *reference : VK_NULL_HANDLE;}
		private:
			T *& reference;
		};



		class Registry : utils::NonCopyable{
		public:
			Registry(const VkInstance & instance);
			~Registry() = default;

			void initialize(VkDevice dev,
				const DeviceQueue & graphics,
				const DeviceQueue &	transfer,
				const DeviceQueue &	present,
				const DeviceQueue &	compute,
				const PhysicalDevice & gpu);

			template<typename T> VkObject<T> * getNew();

			const VkInstance	 & instance;
			const PhysicalDevice & physicalDevice;
			const VkDevice		 & device;
								 
			const DeviceQueue	 & graphicsQueue;
			const DeviceQueue	 & transferQueue;
			const DeviceQueue	 & presentQueue;
			const DeviceQueue	 & computeQueue;

			VkReference<VkCommandPool> transferCommandPool;
			VkReference<VkCommandPool> graphicsCommandPool;
			VkReference<VkCommandPool> computeCommandPool;

		private:
			PhysicalDevice		physicalDevice_m;
			VkDevice 			device_m;

			DeviceQueue			graphicsQueue_m;
			DeviceQueue			transferQueue_m;
			DeviceQueue			presentQueue_m;
			DeviceQueue			computeQueue_m;

			VkCommandPool	  * transferCommandPool_m;
			VkCommandPool	  * graphicsCommandPool_m;
			VkCommandPool	  * computeCommandPool_m;

			template<typename T> inline VkObject<T> * create();
			template<typename T> inline VkObject<T> * create(typename T::Type *& object);
			template<typename T> inline std::function<void(typename T::Type)> getDeleter();
			template<typename T> std::function<void(T)> setDeleterFunction(std::function<void(T, VkAllocationCallbacks*)> deletef);
			template<typename T> std::function<void(T)> setDeleterFunction(std::function<void(VkInstance, T, VkAllocationCallbacks*)> deletef);
			template<typename T> std::function<void(T)> setDeleterFunction(std::function<void(VkDevice, T, VkAllocationCallbacks*)> deletef);

			std::function<void(VkBuffer)>				bufferDeleter;
			std::function<void(VkBufferView)>			bufferViewDeleter; //
			std::function<void(VkCommandPool)>			commandPoolDeleter;
			std::function<void(VkDescriptorPool)>		descriptorPoolDeleter;
			std::function<void(VkDescriptorSetLayout)>	descriptorSetLayoutDeleter;
			std::function<void(VkDeviceMemory)>			deviceMemoryDeleter;
			std::function<void(VkFence)>				fenceDeleter;
			std::function<void(VkFramebuffer)>			frameBufferDeleter;
			std::function<void(VkEvent)>				eventDeleter; //
			std::function<void(VkImage)>				imageDeleter;
			std::function<void(VkImageView)>			imageViewDeleter;
			std::function<void(VkPipelineCache)>		pipelineCacheDeleter;
			std::function<void(VkPipeline)>				pipelineDeleter;
			std::function<void(VkPipelineLayout)>		pipelineLayoutDeleter;
			std::function<void(VkRenderPass)>			renderPassDeleter;
			std::function<void(VkSampler)>				samplerDeleter;
			std::function<void(VkSemaphore)>			semaphoreDeleter;
			std::function<void(VkShaderModule)>			shaderModuleDeleter;
			std::function<void(VkSurfaceKHR)>			surfaceDeleter;
			std::function<void(VkSwapchainKHR)>			swapchainDeleter;
			std::function<void(VkQueryPool)>			queryPoolDeleter; //
		};


		class RegistryManager : utils::NonCopyable {
		public:
			RegistryManager();

			template<typename T> VkObject<T> * getNew();

			Registry & getRegistry();
			
			Registry * createNewRegistry();

			VkReference<VkInstance>		  instance;
			const std::vector<Surface>	& surfaces;

		private:
			std::function<void(VkInstance)>		instanceDeleter = 0;
			std::function<void(VkDevice)>		deviceDeleter = 0;
			std::function<void(VkSurfaceKHR)>	surfaceDeleter = 0;

			VkInstance			  * instance_m;
			std::vector<Surface>	surfaces_m;

			std::vector<Registry*> registrys;
		};
	}
}

