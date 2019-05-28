#pragma once

#include <string>
#include <vector>

#include <vulkan_wrapper.h>

#include <assimp/Importer.hpp> 
#include <assimp/scene.h>     
#include <assimp/postprocess.h>
#include <assimp/cimport.h>

#include <glm/glm.hpp>

#include "Mesh.h"

namespace vkex {
	


	class ModelLoader {
	public:
	private:
		MeshLoader meshloader;
	};


	struct Model {
		struct CreateInfo {

		};
		std::vector<Mesh> meshes;
	};

}

