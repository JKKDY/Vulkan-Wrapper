#pragma once


#include <vulkan_wrapper.h>

#include <assimp/Importer.hpp> 
#include <assimp/scene.h>     
#include <assimp/postprocess.h>
#include <assimp/cimport.h>

#include <glm/glm.hpp>

namespace example {
	class MeshLoader;

	enum VertexComponents : uint32_t {
		VERTEX_COMPONENT_POSITION,
		VERTEX_COMPONENT_NORMAL,
		VERTEX_COMPONENT_COLOR,
		VERTEX_COMPONENT_UV,
		VERTEX_COMPONENT_TANGENT,
		VERTEX_COMPONENT_BITANGENT,
		VERTEX_COMPONENT_DUMMY_FLOAT,
		VERTEX_COMPONENT_DUMMY_VEC4
	};

	struct VertexLayout {
		VertexLayout() = default;
		VertexLayout(const std::vector<VertexComponents> & components);
		const std::vector<VertexComponents> & operator = (const std::vector<VertexComponents> & components);
		uint32_t stride() const;
		static uint8_t sizeOfComponent(VertexComponents component);

		std::vector<VertexComponents> components;
	};



	struct Mesh {
		struct LoadInfo {
			LoadInfo() = default;
			LoadInfo(const std::string & filename, const VertexLayout & layout, Mesh * pMesh) : layout(layout), filePath(filename), pMesh(pMesh) {};
			Mesh * pMesh;
			VertexLayout layout;
			std::string filePath;
			unsigned int assimpFlags = aiProcess_FlipWindingOrder | aiProcess_Triangulate | aiProcess_PreTransformVertices | aiProcess_CalcTangentSpace | aiProcess_GenSmoothNormals;
			glm::vec3 center{ 0.0f };
			glm::vec3 scale{ 1.0f };
			glm::vec2 uvscale{ 1.0f };
		};

		struct Part {
			uint32_t vertexCount;
			uint32_t indexCount;
			uint32_t vertexBase;
			uint32_t indexBase;
		};

		//void clear();

		std::vector<VkVertexInputAttributeDescription> vertexAttributes(uint32_t binding);
		VkVertexInputBindingDescription vertexBinding(uint32_t binding, VkVertexInputRate inputRate = VK_VERTEX_INPUT_RATE_VERTEX);

		uint64_t verticiesSize();
		uint64_t indiciesSize();

		VertexLayout layout;
		std::vector<Part> parts;
		uint32_t indexCount;
		uint32_t vertexCount;
		vkw::SubBuffer vertexBuffer;
		vkw::SubBuffer indexBuffer;
		//MeshLoader * meshLoader;
	};


	class MeshLoader {
		struct FreeSpace {
			uint32_t bufferIndex;
			uint32_t offset;
			uint32_t size;
		};
	public:
		MeshLoader() = default;

		void setDefaultAllocSize(VkDeviceSize size);

		void loadFromFile(const std::vector<Mesh::LoadInfo> & loadInfos);
	private:
		void loadMeshDataIntoMemory(const aiScene * pScene, std::vector<float> & verticies, std::vector<uint32_t> & indicies, const Mesh::LoadInfo & meshloadInfo);

		VkDeviceSize defaultAllocSize = 0;
		std::vector<vkw::Memory> allocations;
		std::vector<vkw::Buffer> indexBuffers;
		std::vector<vkw::Buffer> vertexBuffers;	

		std::map<uint32_t, std::map<uint32_t, VkDeviceSize>> freeSpaces;
	};
}