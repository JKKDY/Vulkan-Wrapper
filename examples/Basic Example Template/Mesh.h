#pragma once

#pragma once
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtx/hash.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>
#include <vector>

#include <vulkan_wraper.h>



struct Vertex {
	glm::vec3 pos;
	glm::vec3 col;
	glm::vec2 tex;
	glm::vec3 norm;

	bool operator==(const Vertex& other) const {
		return pos == other.pos && col == other.col && tex == other.tex && norm == other.norm;
	}
};

namespace std {
	template<> struct hash<Vertex> {
		size_t operator()(Vertex const & vertex) const {
			return (((hash<glm::vec3>()(vertex.pos) ^
				(hash<glm::vec3>()(vertex.col) << 1)) >> 1) ^
				(hash<glm::vec2>()(vertex.tex) << 1) >> 1) ^
				(hash<glm::vec3>()(vertex.norm) << 1);
		}
	};
}


namespace example {
	struct Mesh {
		typedef enum Component {
			POSITION = 1,
			NORMAL = 2,
			UV = 4,
			COLOR = 8
		} Component;
		typedef uint32_t VertComponent;


		Mesh() = default;
		Mesh(std::vector<Vertex> & vertices, std::vector<uint32_t> indices) : vertices(vertices), indices(indices) {};
		Mesh(std::string filePath, VertComponent components) {
			tinyobj::attrib_t attrib;
			std::vector<tinyobj::shape_t> shapes;
			std::vector<tinyobj::material_t> materials;
			std::string err;

			if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &err, filePath.c_str())) {
				throw std::runtime_error(err);
			}

			std::unordered_map<Vertex, uint32_t> uniqueVertices = {};

			for (const auto& shape : shapes) {
				for (const auto& index : shape.mesh.indices) {
					Vertex vertex = {};

					if (components & POSITION) {
						vertex.pos = {
							attrib.vertices[3 * index.vertex_index + 0],
							attrib.vertices[3 * index.vertex_index + 1],
							attrib.vertices[3 * index.vertex_index + 2]
						};
					}
					else {
						vertex.pos = { 0.0, 0.0, 0.0 };
					}


					if (components & NORMAL) {
						vertex.norm = {
							attrib.normals[3 * index.normal_index + 0],
							attrib.normals[3 * index.normal_index + 1],
							attrib.normals[3 * index.normal_index + 2]
						};
					}
					else {
						vertex.norm = { 0.0, 0.0, 0.0 };
					}


					if (components & UV) {
						vertex.tex = {
							attrib.texcoords[2 * index.texcoord_index + 0],
							1.0f - attrib.texcoords[2 * index.texcoord_index + 1]
						};
					}
					else {
						vertex.tex = { 0.0, 0.0 };
					}


					if (components & COLOR) {
						vertex.col = {
							attrib.colors[3 * index.vertex_index + 0],
							attrib.colors[3 * index.vertex_index + 1],
							attrib.colors[3 * index.vertex_index + 2]
						};
					}
					else {
						vertex.col = { 0.0f, 1.0f, 1.0f };
					}


					if (uniqueVertices.count(vertex) == 0) {
						uniqueVertices[vertex] = static_cast<uint32_t>(vertices.size());
						vertices.push_back(vertex);
					}

					indices.push_back(uniqueVertices[vertex]);
				}
			}
		}

		glm::mat4 model;
		std::vector<Vertex> vertices;
		std::vector<uint32_t> indices;

		VkDeviceSize vertOffset;
		VkDeviceSize indOffset;
	};



	struct MeshLoader {

		void loadMesh(std::vector<std::reference_wrapper<Mesh>> models)
		{
			this->meshes = models;
			indexBuffer = vkw::Buffer(VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, indSize());
			vertexBuffer = vkw::Buffer(VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, vertSize());
			indexMemory.allocateMemory({ indexBuffer });
			vertMemory.allocateMemory({ vertexBuffer });

			vkw::Memory stagingMemory(VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
			vkw::Buffer indexStagingBuffer(VK_BUFFER_USAGE_TRANSFER_SRC_BIT, indSize());
			vkw::Buffer vertexStagingBuffer(VK_BUFFER_USAGE_TRANSFER_SRC_BIT, vertSize());
			stagingMemory.allocateMemory({ indexStagingBuffer, vertexStagingBuffer });

			size_t vertOffset = 0;
			size_t indOffset = 0;
			for (Mesh & x : models) {
				size_t indSize = x.indices.size() * sizeof(x.indices[0]);  // size of indices
				indexStagingBuffer.write(x.indices.data(), indSize, indOffset, true);   // write indices to staging buffer

				size_t vertSize = x.vertices.size() * sizeof(Vertex);				// the same just with vertex data
				vertexStagingBuffer.write(x.vertices.data(), vertSize, vertOffset, true);

				x.indOffset = indOffset;
				x.vertOffset = vertOffset;

				indOffset += indSize;
				vertOffset += vertSize;
			}

			stagingMemory.unMap();

			vertexBuffer.copyFromBuffer(vertexStagingBuffer, { 0, 0, vertexStagingBuffer.size });
			indexBuffer.copyFromBuffer(indexStagingBuffer, { 0, 0, indexStagingBuffer.size });
		}

		size_t vertSize() {
			size_t size = 0;
			for (auto x : meshes) {
				size += x.get().vertices.size() * sizeof(Vertex);
			}
			return size;
		}

		size_t indSize() {
			size_t size = 0;
			for (auto x : meshes) {
				size += x.get().indices.size() * sizeof(x.get().indices[0]);
			}
			return size;
		}

		std::vector<std::reference_wrapper<Mesh>> meshes;
		vkw::Memory indexMemory{ VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT };
		vkw::Memory vertMemory{ VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT };
		vkw::Buffer vertexBuffer;
		vkw::Buffer indexBuffer;
	};
}
