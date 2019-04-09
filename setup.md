
# Overview
+ [Requirements](#Requirements)
+ [Setup Guide](#SetupGuide)
+ [File Paths](#FilePaths) 

## Requirements
Building currently only works on windows.

The Vulkan-Wrapper itself requires that the [VulkanSDK](https://vulkan.lunarg.com/) and at least [Visual Studio](https://visualstudio.microsoft.com/de/downloads/) 2017 is installed.

The Examples require the following libraries:
 - [assimp](https://github.com/assimp/assimp)
 - [glfw](https://github.com/glfw/glfw)
 - [stb_image](https://github.com/nothings/stb)
 - [gli](https://github.com/g-truc/gli)
 - [glm](https://github.com/g-truc/glm)
 
## Setup Guide 
1. Download/install/build all dependencies
2. Move Include/library files etc. into their respective directories where compiler and linker can find them (see [File Paths](#FilePaths) for more info)
3. Open [Vulkan Wrapper.sln](VulkanWrapper.sln) with Visual Studio and build

**For step 1 and 2 several scripts have been created to automate this process:** 

[/external/setup_dependencies_vkw.py](/external/setup_dependencies_vkw.py):
 + finds the latest installed [VulkanSDK](https://vulkan.lunarg.com/) on the system and copies alle necessary files into their respective directories
 
 [/examples/external/setup_dependencies_examples.py](/examples/external/setup_dependencies_examples.py):
 + clones* the repositories of [assimp](https://github.com/assimp/assimp), [stb_image](https://github.com/nothings/stb), [gli](https://github.com/g-truc/gli) and [glm](https://github.com/g-truc/glm),
 + downloads [glfw binaries]([https://www.glfw.org/download.html](https://www.glfw.org/download.html)) for windows and copies them into their respective directories.
 + downloads [cmake]([https://github.com/Kitware/CMake](https://github.com/Kitware/CMake)) and automatically builds a Visual Studio solution for assimp for x86 and x64 in /examples/external/assimp/build and /examples/external/assimp/build64, respectively.

[examples/external/assimp/copy_libraries.py](examples/external/assimp/copy_libraries.py):
+ copies assimp binaries in to their respective directories

*this step requires either the installation of git on windows or GitPython. For this [venv.bat](/examples/external/venv.bat) will setup a virtual python envorment in /examples/external/env with GitPython installed 

## File Paths
The paths for compiler and linker for searching the dependencies are stored in two Visual Studio property sheets, [vulkan_client.props](/vulkan_client.props) and [vkw_examples.props](/examples/vkw_examples.props). These can easily be manually changed.

**vulkan_client.props:**

 - used by the wrapper and all examples
 - holds the paths for using:
  [VulkanSDK](https://vulkan.lunarg.com/):
	 -  include files in /external/Vulkan/include
	 - binaries for 32 bit applications in /external/Vulkan/lib32
	 - binaries for 64 bit applications in /external/Vulkan/lib64

**vkw_examples.props**


- used by all examples
- holds the paths for using
- [assimp](https://github.com/assimp/assimp)
	 - include files in /examples/external/assimp/include
	 - binaries for 32 bit applications in /examples/external/assimp/Win32
	 - binaries for 64 bit applications in /examples/external/assimp/Win32
 - [glfw](https://github.com/glfw/glfw)
  	 -  include files in /examples/external/glfw/include
	 - binaries for 32 bit applications in /examples/external/glfw/Win32
	 - binaries for 64 bit applications in /examples/external/glfw/x64
 - [stb_image](https://github.com/nothings/stb)
  	 -  include files in /examples/external/stb
 - [gli](https://github.com/g-truc/gli)
   	 -  include files in /examples/external/gli/gli
 - [glm](https://github.com/g-truc/glm)
     -  include files in /examples/external/glm/glm
