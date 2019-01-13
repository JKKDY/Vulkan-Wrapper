#include "vkw_Foundation.h"

namespace vkw {
	namespace impl {
		RegistryManager::RegistryManager() :
			instanceDeleter([=](VkInstance obj) { vkDestroyInstance(obj, nullptr); }),
			deviceDeleter([=](VkDevice obj) { vkDestroyDevice(obj, nullptr); })
		{
		}

		Registry & RegistryManager::getRegistry()
		{
			return registrys[0];
		}

		void RegistryManager::init(VkInstance * inst)
		{
			//instance.set(inst);
			surfaceDeleter = [=](VkSurfaceKHR obj) {vkDestroySurfaceKHR(*inst, obj, nullptr); };
		}







		void Registry::DeviceQueue::set(VkQueue q, int fam)
		{
			queue = q;
			queueFamily = fam;
		}


		Registry::Registry()
		{

		}

		void Registry::createInstanceDependantDeleters()
		{
			surfaceDeleter = setDeleterFunction	<VkSurfaceKHR>(vkDestroySurfaceKHR);
		}

		void Registry::createDeviceDependantDeleters()
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

		template<> std::function<void(VkInstance)>				 Registry::getDeleter<VkwInstance>() { return instanceDeleter; }
		template<> std::function<void(VkDevice)>				 Registry::getDeleter<VkwDevice>() { return deviceDeleter; }
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

		template<typename T> std::function<void(typename T::Type)>	Registry::getDeleter() { return std::function<void(typename T::Type)>(0); }

		template<typename T> VkObject<T> * Registry::create() { return new VkObject<T>(this->getDeleter<T>(), std::function<void(typename T::Type)>(0)); }
		template<typename T> VkObject<T> * Registry::create(RegObj<T> & obj) { return obj = new VkObject<T>(getDeleter<T>(), obj.getCallback()); }
		template<typename T> VkObject<T> * Registry::create(RegVectorObj<T> & obj) { return obj.add(new VkObject<T>(getDeleter<T>(), obj.getCallback())); }


		template<> VkObject<VkwInstance>			* Registry::getNew() { return create(instance); }
		template<> VkObject<VkwPhysicalDevice>		* Registry::getNew() { return create(physicalDevice); }
		template<> VkObject<VkwDevice>				* Registry::getNew() { return create(device); }
		template<> VkObject<VkwGraphicsCommandPool> * Registry::getNew() { return create(graphicsCommandPool); }
		template<> VkObject<VkwTransferCommandPool> * Registry::getNew() { return create(transferCommandPool); }
		template<> VkObject<VkwSurfaceKHR>			* Registry::getNew() { return create(surfaces); }
												    
		template<> VkObject<VkwBuffer				>* Registry::getNew() { return create<VkwBuffer					>(); }
		template<> VkObject<VkwBufferView			>* Registry::getNew() { return create<VkwBufferView				>(); }
		template<> VkObject<VkwCommandBuffer		>* Registry::getNew() { return create<VkwCommandBuffer			>(); }
		template<> VkObject<VkwCommandPool			>* Registry::getNew() { return create<VkwCommandPool			>(); }
		template<> VkObject<VkwDescriptorPool		>* Registry::getNew() { return create<VkwDescriptorPool			>(); }
		template<> VkObject<VkwDescriptorSetLayout	>* Registry::getNew() { return create<VkwDescriptorSetLayout	>(); }
		template<> VkObject<VkwDescriptorSet		>* Registry::getNew() { return create<VkwDescriptorSet			>(); }
		template<> VkObject<VkwDeviceMemory			>* Registry::getNew() { return create<VkwDeviceMemory			>(); }
		template<> VkObject<VkwFence				>* Registry::getNew() { return create<VkwFence					>(); }
		template<> VkObject<VkwFramebuffer			>* Registry::getNew() { return create<VkwFramebuffer			>(); }
		template<> VkObject<VkwEvent				>* Registry::getNew() { return create<VkwEvent					>(); }
		template<> VkObject<VkwImage				>* Registry::getNew() { return create<VkwImage					>(); }
		template<> VkObject<VkwImageView			>* Registry::getNew() { return create<VkwImageView				>(); }
		template<> VkObject<VkwPipelineCache		>* Registry::getNew() { return create<VkwPipelineCache			>(); }
		template<> VkObject<VkwPipeline				>* Registry::getNew() { return create<VkwPipeline				>(); }
		template<> VkObject<VkwPipelineLayout		>* Registry::getNew() { return create<VkwPipelineLayout			>(); }
		template<> VkObject<VkwRenderPass			>* Registry::getNew() { return create<VkwRenderPass				>(); }
		template<> VkObject<VkwSampler				>* Registry::getNew() { return create<VkwSampler				>(); }
		template<> VkObject<VkwSemaphore			>* Registry::getNew() { return create<VkwSemaphore				>(); }
		template<> VkObject<VkwShaderModule			>* Registry::getNew() { return create<VkwShaderModule			>(); }
		template<> VkObject<VkwSwapchainKHR			>* Registry::getNew() { return create<VkwSwapchainKHR			>(); }
		template<> VkObject<VkwQueryPool			>* Registry::getNew() { return create<VkwQueryPool				>(); }
	}
}