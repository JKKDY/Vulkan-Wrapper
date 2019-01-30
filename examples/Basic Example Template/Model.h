#pragma once
#include <string>
#include <vector>
#include <glm/glm.hpp>
#include <vulkan_wrapper.h>

//#include "Mesh.h"
//#include "Material.h"

namespace exp {

	struct Mesh {
		enum VertexComponent : uint32_t {
			POSITION = 1,
			NORMAL = 2,
			UV = 4,
			COLOR = 8
		};

		struct MeshLoadInfo {
			glm::vec1 * pVerticies;
			uint32_t verticiesCount;
			uint32_t * pIndicies;
			uint32_t indiciesCount;
			VertexComponent components;
			vkw::Buffer *
			vkw::Buffer * vertexStagingBuffer;
			vkw::Buffer * indexStagingBuffer;
		};

		Mesh() = default;

		void loadMesh(glm::vec1 * pVerticies, uint32_t verticiesCount, uint32_t * pIndicies, uint32_t indiciesSize, VertexComponent components, vkw::Buffer & vertexStaging, vkw::Buffer & indexStaging);
	private:
		vkw::SubBuffer vertexBuffer;
		vkw::SubBuffer indexBuffer;
	};



	struct Texture;
	struct Material;

	class Model {
	public:

		struct ModelCreateInfo {
			Mesh * mesh;
			std::vector<Texture*> textures;
			std::vector<Material*> materials;
		};

		Model() = default;
		Model(const ModelCreateInfo & info);
	private:

	};
}