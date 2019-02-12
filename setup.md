**Only works on Windows**


-------------Dependencies-------------
Dependencies needed for the vulkan wrapper:
  VulkanSDK
  
Dependencies needed for the examples:
  assimp:           https://github.com/assimp/assimp.git
  glfw:             https://github.com/glfw/glfw.git
  tinyobjloader:    https://github.com/syoyo/tinyobjloader.git
  stb_image:        https://github.com//nothings/stb.git
  glm:              https://github.com/g-truc/glm
  gli:              https://github.com/g-truc/gli.git
  
  
  
----------------Setup----------------
You can download and setup the dependencies yourself or run the following python files:
  /external/setup_dependencies.py
  /examples/external/setup_dependencies.py
  /examples/external/assimp/copy_libraries.py


/external/setup_dependencies.py:
  Dependencies:
    Python
    VulkanSDK
  
  Description:
    Copies header files and librar files of the VulkanSDK into the /external/Vulkan directory
    
   
/examples/external/setup_dependencies.py:
  Dependencies:
    git
    python + Gitpython*
    
   Description:
    Downloads Github repos of assimp, tinyobjloader, stb_image, glm, gli
    Downloads glfw library
    Downloads cmake
    Creates Visual Studio Project of assimp (32 & 64bit), can then be compiled
    
    
/examples/external/assimp/copy_libraries.py:
  Dependencies:
    Built assimp project (32 & 64bit)
  
  Description:
    copys assimp library files into /examaples/external/assimp/build/ and /examaples/external/assimp/build64/




***************************************
*for the python dependecy Gitpython there is a venv.bat file wich will create a virtual enviroment with this package








