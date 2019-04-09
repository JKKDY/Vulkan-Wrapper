# Vulkan-Wrapper

This is a wrapper for the Vulkan API written in C++. It's main goal is to mitigate some the verbosity of Vulkan by providing a more user-friendly API and automating processes such as allocations and object destruction, while trying to give the user as much creative freedom as possible.

Alongside the Wrapper, there are a couple of examples showcasing the use of the API. These examples are mostly taken directly from [Sasha Willems Vulkan examples](https://github.com/SaschaWillems/Vulkan) and rewritten to use the Vulkan-Wrapper instead of Vulkan. 

# Setup
*A detailed guide on building  and setting up can be found in setup.md*

The Wrapper itself requires that you have the [VulkanSDK](https://vulkan.lunarg.com/) installed and at least [Visual Studio](https://visualstudio.microsoft.com/de/downloads/) 2017.

The Examples use the following libraries:
 - [assimp](https://github.com/assimp/assimp)
 - [glfw](https://github.com/glfw/glfw)
 - [stb_image](https://github.com/nothings/stb)
 - [gli](https://github.com/g-truc/gli)
 - [glm](https://github.com/g-truc/glm)

In the root folder there is a Visual Studio solution for building the Wrapper and running the Examples. For the Wrapper to link against the VulkanSDK, the header files and binaries need to be copied their respective folders "include", "lib32",  "lib64" in [/external/Vulkan](/external/Vulkan/). For the examples to link against their 3rd-Party libraries, these need to be downloaded (and in some cases built) into [/examples/external](/examples/external/). To help with this process two python scripts [/external/setup_dependencies.py](/external/setup_dependencies.py) and [/examples/external/setup_dependencies.py](/examples/external/setup_dependencies.py) have been created. 


 # Design Architecture
 Since there is no documentation (yet), here is quick and very basic overview of the Wrapper:
 
All functions and classes reside in the vkw namespace or have a vkw prefix. (Almost) every Vulkan Handle is wrapped into its own class. This class has the same name as the object, minus the "vk", e.g. ``VkCommandPool -> vkw::CommandPool``. 
 
These "wrapper" classes expose methods for Vulkan-object specific tasks. For instance when vkw::CommandBuffer::submitCommandBuffer is called, it will submit the ``VkCommandBuffer`` of the specific ``vkw::CommandBuffer`` object where it was called from. 
Additionally every object stores data associated with it. For Example ``vkw::Image::extent`` stores the extent of the VkImage.
 
 All of the wrapping classes are subclasses of vkw::Base. This super class automates destruction of Vulkan handles, wich can be influenced by setting ``vkw::Base::destructionControl`` to the desired flag. Furthermore it has a reference to a ``vkw::Registry``. The registry stores information such as the physical device in use (+ properties, features etc.), the logical device, instance and predefined queues and command Buffers for certain operations. With this the user no longer needs to constantly specify information such as the device or instance, since all subclasses of ``vkw::Base`` can just look it up themselves.
 
# Development process and motivation

I started this sometime in 2016/2017 when i was still attending high school because I wanted to learn C++ and Vulkan, but I was somewhat annoyed by the verbosity of Vulkan, so I decided to write library on top to make it easier to use and to further my C++ skills. Since then I work on this project whenever I have time and feel like it. Over the course of development I restarted this project a couple times and made major changes to it but now I think im pretty ok with where it's at currently from design standpoint (who knows what i'll do/think in future``¯\_(ツ)_/¯``) but as far as functionality goes it's still lacking. 


# TO-DOs
Stuff I should probably do ~~someday~~.

- make building process cross-platform
- write documentation
- enable the use of multiple logical devices (currently only theoretically possible)
- implement proper error handling system and debugger
