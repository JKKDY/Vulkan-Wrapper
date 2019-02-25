#pragma once


#include <vulkan_wrapper.h>

#include <assimp/Importer.hpp> 
#include <assimp/scene.h>     
#include <assimp/postprocess.h>
#include <assimp/cimport.h>

#include <glm/glm.hpp>

namespace example {
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
		struct Data {
			void * vertexData;
			VkDeviceSize vertexDataSize;
			void * indexData;
			VkDeviceSize indexDataSize;
		};

		struct Part {
			uint32_t VertexCount;
			uint32_t indexCount;
			uint32_t vertexBase;
			uint32_t indexBase;
		};

		std::vector<VkVertexInputAttributeDescription> getInputDescriptions(uint32_t binding);
		VkVertexInputBindingDescription getBindingDescription(uint32_t binding, VkVertexInputRate inputRate = VK_VERTEX_INPUT_RATE_VERTEX);

		VertexLayout layout;
		std::vector<Part> parts;
		uint32_t indexCount;
		uint32_t vertexCount;
		vkw::SubBuffer vertexSubBuffer;
		vkw::SubBuffer indexSubBuffer;
	};



	struct MeshLoadInfo {
		MeshLoadInfo() = default;
		MeshLoadInfo(const VertexLayout & layout) : layout(layout) {};
		VertexLayout layout;
		unsigned int assimpFlags = aiProcess_FlipWindingOrder | 
			aiProcess_Triangulate | 
			aiProcess_PreTransformVertices | 
			aiProcess_CalcTangentSpace | 
			aiProcess_GenSmoothNormals;
		glm::vec3 center{ 0.0f };
		glm::vec3 scale{ 1.0f };
		glm::vec2 uvscale{ 1.0f };
	};


	class MeshLoader {
	public:
		struct CreateInfo {
			VkDeviceSize defaultVertexBufferSize;
			VkDeviceSize defaultIndexBufferSize;
			VkDeviceSize stagingBufferSize;
		};

		MeshLoader() = default;
		void create(const CreateInfo & info);

		void changeDefaultVertexBufferSize(VkDeviceSize defaultVertexSize);
		void changeDefaultIndexBufferSize(VkDeviceSize defaultIndexSize);

		Mesh loadFromFile(const std::string filename, const MeshLoadInfo & loadInfo);
		void loadFromData(Mesh & mesh, const Mesh::Data & meshData);
	private:
		vkw::Buffer & getVertexBuffer();
		vkw::Buffer & getIndexBuffer();

		uint32_t indexCount = 0;
		uint32_t vertexCount = 0;

		VkDeviceSize defaultVertexSize;
		VkDeviceSize defaultIndexSize;

		vkw::Buffer vertexBuffers;
		vkw::Buffer indexBuffers;

		vkw::Memory allocations;

		vkw::Memory stagingMemory;
		vkw::Buffer stagingBuffer;
	};
}