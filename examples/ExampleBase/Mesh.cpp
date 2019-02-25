#include "Mesh.h"

namespace example {
	VertexLayout::VertexLayout(const std::vector<VertexComponents> & components) :components(components) {}

	const std::vector<VertexComponents>& VertexLayout::operator=(const std::vector<VertexComponents>& components)
	{
		this->components = components;
		return components;
	}

	uint32_t VertexLayout::stride() const
	{
		uint32_t res = 0;
		for (auto & component : components)
		{
			res += sizeOfComponent(component);
		}
		return res;
	}

	uint8_t VertexLayout::sizeOfComponent(VertexComponents component)
	{
		switch (component)
		{
		case VERTEX_COMPONENT_UV:
			return 2 * sizeof(float);
			break;
		case VERTEX_COMPONENT_DUMMY_FLOAT:
			return sizeof(float);
			break;
		case VERTEX_COMPONENT_DUMMY_VEC4:
			return 4 * sizeof(float);
			break;
		default:
			// All components except the ones listed above are made up of 3 floats
			return  3 * sizeof(float);
		}
	}


	std::vector<VkVertexInputAttributeDescription> Mesh::getInputDescriptions(uint32_t binding)
	{
		std::vector<VkVertexInputAttributeDescription> inputDescr (layout.components.size());

		uint32_t offset = 0;

		for (uint32_t i = 0; i < layout.components.size(); i++) {
			inputDescr.at(i).binding = binding;
			inputDescr.at(i).location = i;

			switch (layout.components.at(i))
			{
			case VERTEX_COMPONENT_UV:
				inputDescr.at(i).format = VK_FORMAT_R32G32_SFLOAT;
				break;
			case VERTEX_COMPONENT_DUMMY_FLOAT:
				inputDescr.at(i).format = VK_FORMAT_R32_SFLOAT;
				break;
			case VERTEX_COMPONENT_DUMMY_VEC4:
				inputDescr.at(i).format = VK_FORMAT_R32G32B32A32_SFLOAT;
				break;
			default:
				inputDescr.at(i).format = VK_FORMAT_R32G32B32_SFLOAT;
			}

			offset += VertexLayout::sizeOfComponent(layout.components.at(i));
			inputDescr.at(i).offset = offset;
		}

		return inputDescr;
	}

	VkVertexInputBindingDescription Mesh::getBindingDescription(uint32_t binding, VkVertexInputRate inputRate)
	{
		VkVertexInputBindingDescription bindingDescription = {};
		bindingDescription.binding = binding;
		bindingDescription.inputRate = inputRate;
		bindingDescription.stride = layout.stride();
		return bindingDescription;
	}


	void MeshLoader::create(const CreateInfo & info)
	{
		defaultVertexSize = info.defaultVertexBufferSize;
		defaultIndexSize = info.defaultIndexBufferSize;

		vertexBuffers.createBuffer(VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, defaultVertexSize);
		indexBuffers.createBuffer(VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, defaultVertexSize);

		allocations.allocateMemory({ vertexBuffers, indexBuffers }, {}, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

		stagingBuffer.createBuffer(VK_BUFFER_USAGE_TRANSFER_SRC_BIT, info.stagingBufferSize);
		vkw::Memory::AllocationInfo allocInfo;
		allocInfo.buffers = { stagingBuffer };
		allocInfo.memoryFlags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
		stagingMemory.allocateMemory(allocInfo);
	}

	void MeshLoader::changeDefaultVertexBufferSize(VkDeviceSize defaultVertexBufferSize) { defaultVertexSize = defaultVertexBufferSize; }

	void MeshLoader::changeDefaultIndexBufferSize(VkDeviceSize defaultIndexBufferSize) { defaultIndexSize = defaultIndexBufferSize; }

	Mesh MeshLoader::loadFromFile(const std::string filename, const MeshLoadInfo & loadInfo)
	{
		Mesh mesh = {};
		mesh.layout = loadInfo.layout;

		Assimp::Importer importer;
		const aiScene * pScene = importer.ReadFile(filename.c_str(), loadInfo.assimpFlags);
		
		VKW_assert(pScene, importer.GetErrorString());

		mesh.parts.clear();
		mesh.parts.resize(pScene->mNumMeshes);

		std::vector<float> vertexBuffer;
		std::vector<uint32_t> indexBuffer;

		vertexCount = 0;
		indexCount = 0;


		for (unsigned int i = 0; i < pScene->mNumMeshes; i++)
		{
			const aiMesh* paiMesh = pScene->mMeshes[i];

			mesh.parts[i] = {};
			mesh.parts[i].vertexBase = vertexCount;
			mesh.parts[i].indexBase = indexCount;

			vertexCount += pScene->mMeshes[i]->mNumVertices;

			aiColor3D pColor(0.f, 0.f, 0.f);
			pScene->mMaterials[paiMesh->mMaterialIndex]->Get(AI_MATKEY_COLOR_DIFFUSE, pColor);

			const aiVector3D Zero3D(0.0f, 0.0f, 0.0f);

			for (unsigned int j = 0; j < paiMesh->mNumVertices; j++)
			{
				const aiVector3D* pPos = &(paiMesh->mVertices[j]);
				const aiVector3D* pNormal = &(paiMesh->mNormals[j]);
				const aiVector3D* pTexCoord = (paiMesh->HasTextureCoords(0)) ? &(paiMesh->mTextureCoords[0][j]) : &Zero3D;
				const aiVector3D* pTangent = (paiMesh->HasTangentsAndBitangents()) ? &(paiMesh->mTangents[j]) : &Zero3D;
				const aiVector3D* pBiTangent = (paiMesh->HasTangentsAndBitangents()) ? &(paiMesh->mBitangents[j]) : &Zero3D;

				for (auto& component : mesh.layout.components)
				{
					switch (component) {
					case VERTEX_COMPONENT_POSITION:
						vertexBuffer.push_back(pPos->x * loadInfo.scale.x + loadInfo.center.x);
						vertexBuffer.push_back(-pPos->y * loadInfo.scale.y + loadInfo.center.y);
						vertexBuffer.push_back(pPos->z * loadInfo.scale.z + loadInfo.center.z);
						break;
					case VERTEX_COMPONENT_NORMAL:
						vertexBuffer.push_back(pNormal->x);
						vertexBuffer.push_back(-pNormal->y);
						vertexBuffer.push_back(pNormal->z);
						break;
					case VERTEX_COMPONENT_UV:
						vertexBuffer.push_back(pTexCoord->x * loadInfo.uvscale.s);
						vertexBuffer.push_back(pTexCoord->y * loadInfo.uvscale.t);
						break;
					case VERTEX_COMPONENT_COLOR:
						vertexBuffer.push_back(pColor.r);
						vertexBuffer.push_back(pColor.g);
						vertexBuffer.push_back(pColor.b);
						break;
					case VERTEX_COMPONENT_TANGENT:
						vertexBuffer.push_back(pTangent->x);
						vertexBuffer.push_back(pTangent->y);
						vertexBuffer.push_back(pTangent->z);
						break;
					case VERTEX_COMPONENT_BITANGENT:
						vertexBuffer.push_back(pBiTangent->x);
						vertexBuffer.push_back(pBiTangent->y);
						vertexBuffer.push_back(pBiTangent->z);
						break;
						// Dummy components for padding
					case VERTEX_COMPONENT_DUMMY_FLOAT:
						vertexBuffer.push_back(0.0f);
						break;
					case VERTEX_COMPONENT_DUMMY_VEC4:
						vertexBuffer.push_back(0.0f);
						vertexBuffer.push_back(0.0f);
						vertexBuffer.push_back(0.0f);
						vertexBuffer.push_back(0.0f);
						break;
					};
				}
			}

			mesh.parts[i].VertexCount = paiMesh->mNumVertices;

			uint32_t indexBase = static_cast<uint32_t>(indexBuffer.size());
			for (unsigned int j = 0; j < paiMesh->mNumFaces; j++)
			{
				const aiFace& Face = paiMesh->mFaces[j];
				if (Face.mNumIndices != 3)
					continue;
				indexBuffer.push_back(indexBase + Face.mIndices[0]);
				indexBuffer.push_back(indexBase + Face.mIndices[1]);
				indexBuffer.push_back(indexBase + Face.mIndices[2]);
				mesh.parts[i].indexCount += 3;
				indexCount += 3;
			}
		}

		mesh.indexCount = static_cast<uint32_t>(indexBuffer.size());
		mesh.vertexCount = static_cast<uint32_t>((vertexBuffer.size() * sizeof(float)) / mesh.layout.stride());

		Mesh::Data data = {};
		data.indexData = indexBuffer.data();
		data.indexDataSize = indexBuffer.size() * sizeof(uint32_t);
		data.vertexData = vertexBuffer.data();
		data.vertexDataSize = vertexBuffer.size() * sizeof(float);
		loadFromData(mesh, data);

		return mesh;
	}

	void MeshLoader::loadFromData(Mesh & mesh, const Mesh::Data & meshData)
	{
		vkw::SubBuffer stagingVertex = stagingBuffer.createSubBuffer(meshData.vertexDataSize);
		vkw::SubBuffer stagingIndex = stagingBuffer.createSubBuffer(meshData.indexDataSize);
		stagingVertex.write(meshData.vertexData, meshData.vertexDataSize);
		stagingIndex.write(meshData.indexData, meshData.indexDataSize);

		mesh.vertexSubBuffer = getVertexBuffer().createSubBuffer(meshData.vertexDataSize);
		mesh.vertexSubBuffer.copyFrom(stagingVertex);

		mesh.indexSubBuffer = getIndexBuffer().createSubBuffer(meshData.indexDataSize);
		mesh.indexSubBuffer.copyFrom(stagingIndex);

		stagingIndex.clear();
		stagingVertex.clear();
	}

	vkw::Buffer & MeshLoader::getVertexBuffer()
	{
		return vertexBuffers;
	}

	vkw::Buffer & MeshLoader::getIndexBuffer()
	{
		return indexBuffers;
	}
}