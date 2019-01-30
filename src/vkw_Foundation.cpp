#include "vkw_Foundation.h"
#include "vkw_Core.h"

namespace vkw {

	namespace impl {
		static RegistryManager registryManager;

		template <typename Reg> Reg & getRegistry();

		template <> RegistryManager & getRegistry() { return registryManager; }
		template <> Registry & getRegistry() { return registryManager.getRegistry(); }





		RegistryManager::RegistryManager():
			instanceDeleter ([=](VkInstance obj) { vkDestroyInstance(obj, nullptr); }),
			deviceDeleter([=](VkDevice obj) { vkDestroyDevice(obj, nullptr); }),
			instance(instance_m),
			surfaces(surfaces_m)
		{
		}

		template<> VkObject<VkwInstance>* RegistryManager::getNew() { 
			VkObject<VkwInstance> * obj = new VkObject<VkwInstance>(instanceDeleter, [](){});
			instance_m = obj->getPointer();
			return obj;
		}

		template<> VkObject<VkwDevice>* RegistryManager::getNew() { 
			return new VkObject<VkwDevice>(deviceDeleter, [](){}); 
		}

		template<> VkObject<VkwSurfaceKHR>* RegistryManager::getNew() { 
			if (!surfaceDeleter) surfaceDeleter = [=](VkSurfaceKHR obj) {vkDestroySurfaceKHR(instance, obj, nullptr); };
			return new VkObject<VkwSurfaceKHR>(surfaceDeleter, [](){}); 
		}

		Registry & RegistryManager::getRegistry()
		{
			return *registrys[0];
		}

		Registry * RegistryManager::createNewRegistry(VkDevice dev,
			const DeviceQueue & graphics,
			const DeviceQueue &	transfer,
			const DeviceQueue &	present,
			const DeviceQueue &	compute,
			const PhysicalDevice & gpu)
		{
			Registry * reg = new Registry(*instance_m, dev, graphics, transfer, present, compute, gpu);

			registrys.push_back(reg);

			return reg;
		}




		PhysicalDevice::operator VkPhysicalDevice() const
		{
			return physicalDevice;
		}

		Surface::operator VkSurfaceKHR() const
		{
			return surface;
		}

		DeviceQueue::operator VkQueue() const
		{
			return queue;
		}



	
		Registry::Registry(const VkInstance & instance,
			VkDevice dev,
			const DeviceQueue & graphics,
			const DeviceQueue & transfer,
			const DeviceQueue & present,
			const DeviceQueue & compute,
			const PhysicalDevice & gpu):
			instance(instance),
			physicalDevice(physicalDevice_m),
			device(device_m),
			transferCommandPool(transferCommandPool_m),
			graphicsCommandPool(graphicsCommandPool_m),
			computeCommandPool(computeCommandPool_m),
			graphicsQueue(graphicsQueue_m),
			transferQueue(transferQueue_m),
			presentQueue(presentQueue_m),
			computeQueue(computeQueue_m),
			device_m(dev),
			physicalDevice_m(gpu),
			graphicsQueue_m(graphics),
			transferQueue_m(transfer),
			presentQueue_m(present),
			computeQueue_m(compute)
		{
			fenceDeleter = setDeleterFunction	<VkFence>(vkDestroyFence);
			semaphoreDeleter = setDeleterFunction	<VkSemaphore>(vkDestroySemaphore);
			eventDeleter = setDeleterFunction	<VkEvent>(vkDestroyEvent);
			queryPoolDeleter = setDeleterFunction	<VkQueryPool>(vkDestroyQueryPool);
			bufferDeleter = setDeleterFunction	<VkBuffer>(vkDestroyBuffer);
			bufferViewDeleter = setDeleterFunction	<VkBufferView>(vkDestroyBufferView);
			imageDeleter = setDeleterFunction	<VkImage>(vkDestroyImage);
			imageViewDeleter = setDeleterFunction	<VkImageView>(vkDestroyImageView);
			deviceMemoryDeleter = setDeleterFunction	<VkDeviceMemory>(vkFreeMemory);
			shaderModuleDeleter = setDeleterFunction	<VkShaderModule>(vkDestroyShaderModule);
			pipelineCacheDeleter = setDeleterFunction	<VkPipelineCache>(vkDestroyPipelineCache);
			pipelineDeleter = setDeleterFunction	<VkPipeline>(vkDestroyPipeline);
			pipelineLayoutDeleter = setDeleterFunction	<VkPipelineLayout>(vkDestroyPipelineLayout);
			samplerDeleter = setDeleterFunction	<VkSampler>(vkDestroySampler);
			descriptorSetLayoutDeleter = setDeleterFunction	<VkDescriptorSetLayout>(vkDestroyDescriptorSetLayout);
			descriptorPoolDeleter = setDeleterFunction	<VkDescriptorPool>(vkDestroyDescriptorPool);
			frameBufferDeleter = setDeleterFunction	<VkFramebuffer>(vkDestroyFramebuffer);
			renderPassDeleter = setDeleterFunction	<VkRenderPass>(vkDestroyRenderPass);
			commandPoolDeleter = setDeleterFunction	<VkCommandPool>(vkDestroyCommandPool);
			swapchainDeleter = setDeleterFunction	<VkSwapchainKHR>(vkDestroySwapchainKHR);
		}

		template<typename T> std::function<void(T)> Registry::setDeleterFunction(std::function<void(T, VkAllocationCallbacks*)> deletef)
		{
			return [=](T obj) {deletef(obj, nullptr); };
		}

		template<typename T> std::function<void(T)> Registry::setDeleterFunction(std::function<void(VkInstance, T, VkAllocationCallbacks*)> deletef)
		{
			return [=](T obj) {deletef(instance, obj, nullptr); };
		}

		template<typename T> std::function<void(T)> Registry::setDeleterFunction(std::function<void(VkDevice, T, VkAllocationCallbacks*)> deletef)
		{
			return [=](T obj) {deletef(device, obj, nullptr); };
		}

	/*	template<> std::function<void(VkInstance)>				 Registry::getDeleter<VkwInstance>() { return instanceDeleter; }
		template<> std::function<void(VkDevice)>				 Registry::getDeleter<VkwDevice>() { return deviceDeleter; }*/
		template<> std::function<void(VkBuffer)>				 Registry::getDeleter<VkwBuffer>() { return bufferDeleter; }
		template<> std::function<void(VkBufferView)>			 Registry::getDeleter<VkwBufferView>() { return bufferViewDeleter; }
		template<> std::function<void(VkCommandPool)>			 Registry::getDeleter<VkwCommandPool>() { return commandPoolDeleter; }
		template<> std::function<void(VkDescriptorPool)>		 Registry::getDeleter<VkwDescriptorPool>() { return descriptorPoolDeleter; }
		template<> std::function<void(VkDescriptorSetLayout)>	 Registry::getDeleter<VkwDescriptorSetLayout>() { return descriptorSetLayoutDeleter; }
		template<> std::function<void(VkDeviceMemory)>			 Registry::getDeleter<VkwDeviceMemory>() { return deviceMemoryDeleter; }
		template<> std::function<void(VkFence)>					 Registry::getDeleter<VkwFence>() { return fenceDeleter; }
		template<> std::function<void(VkFramebuffer)>			 Registry::getDeleter<VkwFramebuffer>() { return frameBufferDeleter; }
		template<> std::function<void(VkEvent)>					 Registry::getDeleter<VkwEvent>() { return eventDeleter; }
		template<> std::function<void(VkImage)>					 Registry::getDeleter<VkwImage>() { return imageDeleter; }
		template<> std::function<void(VkImageView)>				 Registry::getDeleter<VkwImageView>() { return imageViewDeleter; }
		template<> std::function<void(VkPipelineCache)>			 Registry::getDeleter<VkwPipelineCache>() { return pipelineCacheDeleter; }
		template<> std::function<void(VkPipeline)>				 Registry::getDeleter<VkwPipeline>() { return pipelineDeleter; }
		template<> std::function<void(VkPipelineLayout)>		 Registry::getDeleter<VkwPipelineLayout>() { return pipelineLayoutDeleter; }
		template<> std::function<void(VkRenderPass)>			 Registry::getDeleter<VkwRenderPass>() { return renderPassDeleter; }
		template<> std::function<void(VkSampler)>				 Registry::getDeleter<VkwSampler>() { return samplerDeleter; }
		template<> std::function<void(VkSemaphore)>				 Registry::getDeleter<VkwSemaphore>() { return semaphoreDeleter; }
		template<> std::function<void(VkShaderModule)>			 Registry::getDeleter<VkwShaderModule>() { return shaderModuleDeleter; }
		template<> std::function<void(VkSurfaceKHR)>			 Registry::getDeleter<VkwSurfaceKHR>() { return surfaceDeleter; }
		template<> std::function<void(VkSwapchainKHR)>			 Registry::getDeleter<VkwSwapchainKHR>() { return swapchainDeleter; }
		template<> std::function<void(VkQueryPool)>				 Registry::getDeleter<VkwQueryPool>() { return queryPoolDeleter; }

		template<> std::function<void(VkCommandPool)>			 Registry::getDeleter<VkwGraphicsCommandPool>() { return commandPoolDeleter; }
		template<> std::function<void(VkCommandPool)>			 Registry::getDeleter<VkwTransferCommandPool>() { return commandPoolDeleter; }
		template<> std::function<void(VkCommandPool)>			 Registry::getDeleter<VkwComputeCommandPool>() { return commandPoolDeleter; }

		template<typename T> std::function<void(typename T::Type)>	Registry::getDeleter() { return std::function<void(typename T::Type)>(0); }

		template<typename T> VkObject<T> * Registry::create() { return new VkObject<T>(getDeleter<T>(), std::function<void(void)> (0)); }

		template<typename T> VkObject<T> * Registry::create(typename T::Type *& object) { 
			VkObject<T> * obj = new VkObject<T>(getDeleter<T>(), [&]() { object = nullptr; });
			object = obj->getPointer();
			return obj;
		}

		template<> VkObject<VkwGraphicsCommandPool> * Registry::getNew() { return create<VkwGraphicsCommandPool>(graphicsCommandPool_m); }
		template<> VkObject<VkwTransferCommandPool> * Registry::getNew() { return create<VkwTransferCommandPool>(transferCommandPool_m); }
		template<> VkObject<VkwComputeCommandPool> * Registry::getNew() { return create<VkwComputeCommandPool>(computeCommandPool_m); }

		template<typename T> VkObject<T>* Registry::getNew() { return create<T>(); }
			
		
		




		

		/// Vk Object
		template<typename T> VkObject<T>::VkObject(std::function<void(Type)> & delf, std::function<void(void)> callback):
			deleterCallback(callback),
			deleterFunc(delf)
		{}

		template<typename T> VkObject<T>::~VkObject()
		{
			if (deleterCallback) deleterCallback();
		}

		template<typename T> void VkObject<T>::add(VkObject<T> *& ref) {
			references.push_back(&ref);
		}

		template<typename T> void VkObject<T>::remove(VkObject<T> *& ref) {
			references.erase(std::remove(references.begin(), references.end(), &ref), references.end());
			ref = nullptr;

			if (references.size() == 0) {
				if (object != VK_NULL_HANDLE && deleterFunc != 0) {
					deleterFunc(object);
				}

				delete this;
			}
		}

		template<typename T> void VkObject<T>::deleteThis(std::function<void(Type)> deleterf)
		{
			if (object != VK_NULL_HANDLE) {
				if (deleterf != 0) {
					deleterf(object);
				}
				else if (deleterFunc != 0) {
					deleterFunc(object);
				}

				object = VK_NULL_HANDLE;
			}

			for (auto x : references) {
				*x = nullptr;
			}

			delete this;
		}

		template<typename T> uint32_t VkObject<T>::referenceCount()
		{
			return static_cast<uint32_t>(references.size());
		}

		template<typename T> typename T::Type * VkObject<T>::getPointer()
		{
			return &object;
		}

		template<class T> typename T::Type VkObject<T>::getObject()
		{
			return object;
		}

		template<typename T> VkObject<T>::operator typename T::Type()
		{
			return object;
		}

		template<typename T> void VkObject<T>::operator = (const Type & rhs) {
			object = rhs;
		}




		
		/// Vk Pointer
		template<typename T, typename RegType> VkPointer<T, RegType>::VkPointer(RegType & reg) :
			registry(reg)
		{}

		template<typename T, typename RegType> VkPointer<T, RegType>::VkPointer(const VkPointer<T, RegType> & obj) :
			pObject(obj.pObject),
			registry(obj.registry)
		{
			if (!pObject) {
				pObject = registry.getNew<T>();
				obj.pObject = pObject;
				pObject->add(pObject);
				obj.pObject->add(obj.pObject);
			}
			else {
				pObject->add(pObject);
			}
		}

		template<typename T, typename RegType> void VkPointer<T, RegType>::copy(const VkPointer<T, RegType> & obj, DestructionControl destrContr)
		{
			pObject->remove(pObject);
			
			pObject = obj.pObject;

			pObject->add(pObject); 
		}

		template<typename T, typename RegType> void VkPointer<T, RegType>::destroyObject(DestructionControl destrContr, std::function<void(Type)> deleterf) // gets called manualy
		{
			if (pObject) {
				if (destrContr == VKW_DESTR_CONTRL_FIRST_OBJECT_CALLS_DELETER ||
					destrContr == VKW_DESTR_CONTRL_EXCLUSIVE_DELETER_CALL ||
					destrContr == VKW_DESTR_CONTRL_LAST_OBJECT_CALLS_DELETER && pObject->referenceCount() <= 1)
				{
					pObject->deleteThis(deleterf); // set to nullptr automatically
				}
				else {
					pObject->remove(pObject); // set to nullptr automatically
				}
			}
		}

		template<typename T, typename RegType> void VkPointer<T, RegType>::createNewObject() const
		{
			if (!pObject) {
				pObject = registry.getNew<T>();
				pObject->add(pObject);
			}
		}

		template<typename T, typename RegType> void VkPointer<T, RegType>::operator = (Type rhs) {
			createNewObject();
			*pObject = rhs;
		}

	/*	template<typename T, typename RegType> VkPointer<T, RegType>& VkPointer<T, RegType>::operator=(VkPointer<T, RegType>& rhs)
		{
			rhs.pObject->add(pObject);
			pObject = rhs.pObject;
			rhs.pObject->remove(rhs.pObject);

			return *this;
		}*/

		template<typename T, typename RegType> VkPointer<T, RegType>::operator Type*() const
		{
			createNewObject();
			return pObject->getPointer();
		}

		template<typename T, typename RegType> typename T::Type VkPointer<T, RegType>::operator * () const
		{
			createNewObject();
			return pObject->getObject();
		}





		/// Vk Object
		template<typename T, typename RegType> Base<T, RegType>::Base() :
			registry(getRegistry<RegType>()),
			vkObject(registry)
		{}

		template<typename T, typename RegType> Base<T, RegType>::Base(const Base<T, RegType> & rhs):
			registry(rhs.registry),
			vkObject(rhs.vkObject),
			passOnVkObject(rhs.passOnVkObject),
			destructionControl((rhs.destructionControl == VKW_DESTR_CONTRL_EXCLUSIVE_DELETER_CALL && passOnVkObject) ? VKW_DESTR_CONTRL_DO_NOTHING : rhs.destructionControl)
		{}
		
		template<typename T, typename RegType> Base<T, RegType>::~Base()
		{
			destroyObject();
		}

		template<typename T, typename RegType> Base<T, RegType> & Base<T, RegType>::operator = (const Base<T, RegType> & rhs)
		{
			passOnVkObject = rhs.passOnVkObject;

			if (passOnVkObject) {
				vkObject.copy(rhs.vkObject, destructionControl);

				destructionControl = (rhs.destructionControl == VKW_DESTR_CONTRL_EXCLUSIVE_DELETER_CALL && passOnVkObject) ? VKW_DESTR_CONTRL_DO_NOTHING : rhs.destructionControl;
			} 

			return *this;
		}

	/*	template<typename T, typename RegType> Base<T, RegType>& Base<T, RegType>::operator=(Base<T, RegType>&& rhs)
		{
			std::cout << "poop";
			passOnVkObject = true;
			DestructionControl destructionControl = VKW_DESTR_CONTRL_LAST_OBJECT_CALLS_DELETER;

			std::swap(vkObject, rhs.vkObject);

			return *this;
		}*/

		template<typename T, typename RegType> void Base<T, RegType>::destroyObject()
		{
			vkObject.destroyObject(destructionControl);
		}

		template<typename T, typename RegType>  Base<T, RegType>::operator typename T::Type () const
		{
			return *vkObject;
		}

		template<typename T, typename RegType> typename T::Type * Base<T, RegType>::get() const
		{
			return vkObject;
		}







		template class VkObject<VkwBuffer>;
		template class VkObject<VkwCommandBuffer>;
		template class VkObject<VkwCommandPool>;
		template class VkObject<VkwDescriptorPool>;
		template class VkObject<VkwDescriptorSetLayout>;
		template class VkObject<VkwDescriptorSet>;
		template class VkObject<VkwDevice>;
		template class VkObject<VkwDeviceMemory>;
		template class VkObject<VkwFence>;
		template class VkObject<VkwFramebuffer>;
		template class VkObject<VkwImage>;
		template class VkObject<VkwImageView>;
		template class VkObject<VkwInstance>;
		template class VkObject<VkwPhysicalDevice>;
		template class VkObject<VkwPipelineCache>;
		template class VkObject<VkwPipeline>;
		template class VkObject<VkwPipelineLayout>;
		template class VkObject<VkwRenderPass>;
		template class VkObject<VkwSampler>;
		template class VkObject<VkwSemaphore>;
		template class VkObject<VkwShaderModule>;
		template class VkObject<VkwSurfaceKHR>;
		template class VkObject<VkwSwapchainKHR>;
		template class VkObject<VkwGraphicsCommandPool>;
		template class VkObject<VkwTransferCommandPool>;
		template class VkObject<VkwComputeCommandPool>;

		template class VkPointer<VkwInstance, RegistryManager>;
		template class VkPointer<VkwDevice, RegistryManager>;
		template class VkPointer<VkwSurfaceKHR, RegistryManager>;

		template class VkPointer<VkwBuffer, Registry>;
		template class VkPointer<VkwCommandBuffer, Registry>;
		template class VkPointer<VkwCommandPool, Registry>;
		template class VkPointer<VkwDescriptorPool, Registry>;
		template class VkPointer<VkwDescriptorSetLayout, Registry>;
		template class VkPointer<VkwDescriptorSet, Registry>;
		template class VkPointer<VkwDeviceMemory, Registry>;
		template class VkPointer<VkwFence, Registry>;
		template class VkPointer<VkwFramebuffer, Registry>;
		template class VkPointer<VkwImage, Registry>;
		template class VkPointer<VkwImageView, Registry>;
		template class VkPointer<VkwPhysicalDevice, Registry>;
		template class VkPointer<VkwPipelineCache, Registry>;
		template class VkPointer<VkwPipeline, Registry>;
		template class VkPointer<VkwPipelineLayout, Registry>;
		template class VkPointer<VkwRenderPass, Registry>;
		template class VkPointer<VkwSampler, Registry>;
		template class VkPointer<VkwSemaphore, Registry>;
		template class VkPointer<VkwShaderModule, Registry>;
		template class VkPointer<VkwSwapchainKHR, Registry>;
		template class VkPointer<VkwGraphicsCommandPool, Registry>;
		template class VkPointer<VkwTransferCommandPool, Registry>;
		template class VkPointer<VkwComputeCommandPool, Registry>;


		template class Base<VkwInstance, RegistryManager>;
		template class Base<VkwSurfaceKHR, RegistryManager>;
		template class Base<VkwDevice, RegistryManager>;

		template class Base<VkwBuffer, Registry>;
		template class Base<VkwCommandBuffer, Registry>;
		template class Base<VkwCommandPool, Registry>;
		template class Base<VkwDescriptorPool, Registry>;
		template class Base<VkwDescriptorSetLayout, Registry>;
		template class Base<VkwDescriptorSet, Registry>;
		template class Base<VkwDeviceMemory, Registry>;
		template class Base<VkwFence, Registry>;
		template class Base<VkwFramebuffer, Registry>;
		template class Base<VkwImage, Registry>;
		template class Base<VkwImageView, Registry>;
		template class Base<VkwPhysicalDevice, Registry>;
		template class Base<VkwPipelineCache, Registry>;
		template class Base<VkwPipeline, Registry>;
		template class Base<VkwPipelineLayout, Registry>;
		template class Base<VkwRenderPass, Registry>;
		template class Base<VkwSampler, Registry>;
		template class Base<VkwSemaphore, Registry>;
		template class Base<VkwShaderModule, Registry>;
		template class Base<VkwSwapchainKHR, Registry>;
		template class Base<VkwGraphicsCommandPool, Registry>;
		template class Base<VkwTransferCommandPool, Registry>;
		template class Base<VkwComputeCommandPool, Registry>;
	}
}