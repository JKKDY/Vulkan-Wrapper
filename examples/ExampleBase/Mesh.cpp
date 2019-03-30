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


	std::vector<VkVertexInputAttributeDescription> Mesh::vertexAttributes(uint32_t binding)
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

			inputDescr.at(i).offset = offset;
			offset += VertexLayout::sizeOfComponent(layout.components.at(i));
		}

		return inputDescr;
	}

	VkVertexInputBindingDescription Mesh::vertexBinding(uint32_t binding, VkVertexInputRate inputRate)
	{
		VkVertexInputBindingDescription bindingDescription = {};
		bindingDescription.binding = binding;
		bindingDescription.inputRate = inputRate;
		bindingDescription.stride = layout.stride();
		return bindingDescription;
	}

	uint64_t Mesh::verticiesSize()
	{
		return vertexCount * layout.stride();
	}

	uint64_t Mesh::indiciesSize()
	{
		return indexCount * sizeof(uint32_t);
	}

	void MeshLoader::setDefaultAllocSize(VkDeviceSize size)
	{
		defaultAllocSize = size;
	}

	void MeshLoader::loadFromFile(const std::vector<Mesh::LoadInfo> & loadInfos)
	{
		Assimp::Importer importer;
		VkDeviceSize totalVertSize = 0;
		VkDeviceSize totalIndSize = 0;

		struct BufferLoadInfo {
			Mesh * mesh;
			std::vector<float> verticies;
			std::vector<uint32_t> indicies;
			vkw::SubBuffer vertStagingSubBuffer;
			vkw::SubBuffer indStagingSubBuffer;
		};
		std::vector<BufferLoadInfo> bufferLoadInfos(loadInfos.size());



		// load into RAM
		for (unsigned int i = 0; i < loadInfos.size(); i++) {
			const aiScene * pScene = importer.ReadFile(loadInfos.at(i).filePath.c_str(), loadInfos.at(i).assimpFlags);
			VKW_assert(pScene, importer.GetErrorString());
	
			loadMeshDataIntoMemory(pScene, bufferLoadInfos.at(i).verticies, bufferLoadInfos.at(i).indicies, loadInfos.at(i));

			bufferLoadInfos.at(i).mesh = loadInfos.at(i).pMesh;
			bufferLoadInfos.at(i).mesh->layout = loadInfos.at(i).layout;

			totalVertSize += bufferLoadInfos.at(i).mesh->verticiesSize();
			totalIndSize += bufferLoadInfos.at(i).mesh->indiciesSize();
		}
		

		// prepare 
		indexBuffers.emplace_back( VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,  totalIndSize);
		vertexBuffers.emplace_back( VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,  totalVertSize);

		bool needToAllocate = true;
		for (auto & x : allocations) {
			if (x.memoryRanges.query(indexBuffers.back().sizeInMemory + vertexBuffers.back().sizeInMemory)) {
				x.bindBufferToMemory(indexBuffers.back());
				x.bindBufferToMemory(vertexBuffers.back());
				needToAllocate = false;
				break;
			}
		}

		if (needToAllocate) {
			vkw::Memory::AllocInfo allocInfo = {};
			allocInfo.buffers = { indexBuffers.back(), vertexBuffers.back() };
			allocInfo.memoryFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
			allocInfo.additionalSize = defaultAllocSize == 0 ? 0 : std::max(defaultAllocSize - (indexBuffers.back().sizeInMemory + vertexBuffers.back().sizeInMemory), (unsigned long long)0);
			allocations.emplace_back(allocInfo);
		}

		vkw::Memory stagingMemory(VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
		vkw::Buffer stagingBuffer(VK_BUFFER_USAGE_TRANSFER_SRC_BIT, totalVertSize + totalIndSize);
		stagingMemory.allocateMemory({ stagingBuffer });



		// load into buffers
		for (auto & x : bufferLoadInfos) {
			Mesh & mesh = *x.mesh;
			x.vertStagingSubBuffer = stagingBuffer.createSubBuffer(mesh.verticiesSize());
			x.vertStagingSubBuffer.write(x.verticies.data(), mesh.verticiesSize());

			x.indStagingSubBuffer = stagingBuffer.createSubBuffer(mesh.indiciesSize());
			x.indStagingSubBuffer.write(x.indicies.data(), mesh.indiciesSize());

			mesh.vertexBuffer = vertexBuffers.back().createSubBuffer(mesh.verticiesSize());
			mesh.vertexBuffer.copyFrom(x.vertStagingSubBuffer);

			mesh.indexBuffer = indexBuffers.back().createSubBuffer(mesh.indiciesSize());
			mesh.indexBuffer.copyFrom(x.indStagingSubBuffer);

			x.indStagingSubBuffer.clear();
			x.vertStagingSubBuffer.clear();
		}
	}

	void MeshLoader::loadMeshDataIntoMemory(const aiScene * pScene, std::vector<float>& verticies, std::vector<uint32_t> & indicies, const Mesh::LoadInfo & meshloadInfo)
	{
		Mesh & mesh = *meshloadInfo.pMesh;
		const aiVector3D Zero3D(0.0f, 0.0f, 0.0f);
		mesh.parts.clear();
		mesh.parts.resize(pScene->mNumMeshes);
		uint32_t vertexCount = 0;
		uint32_t indexCount = 0;

		for (unsigned int i = 0; i < pScene->mNumMeshes; i++)
		{
			const aiMesh* paiMesh = pScene->mMeshes[i];

			mesh.parts[i].vertexBase = vertexCount;
			mesh.parts[i].indexBase = indexCount;

			aiColor3D pColor(0.f, 0.f, 0.f);
			pScene->mMaterials[paiMesh->mMaterialIndex]->Get(AI_MATKEY_COLOR_DIFFUSE, pColor);

			vertexCount += pScene->mMeshes[i]->mNumVertices;
			mesh.parts[i].vertexCount = paiMesh->mNumVertices;

			// load verticies
			for (unsigned int j = 0; j < paiMesh->mNumVertices; j++)
			{
				const aiVector3D* pPos = &(paiMesh->mVertices[j]);
				const aiVector3D* pNormal = &(paiMesh->mNormals[j]);
				const aiVector3D* pTexCoord = (paiMesh->HasTextureCoords(0)) ? &(paiMesh->mTextureCoords[0][j]) : &Zero3D;
				const aiVector3D* pTangent = (paiMesh->HasTangentsAndBitangents()) ? &(paiMesh->mTangents[j]) : &Zero3D;
				const aiVector3D* pBiTangent = (paiMesh->HasTangentsAndBitangents()) ? &(paiMesh->mBitangents[j]) : &Zero3D;

				for (auto& component : meshloadInfo.layout.components)
				{
					switch (component) {
					case VERTEX_COMPONENT_POSITION:
						verticies.push_back(pPos->x * meshloadInfo.scale.x + meshloadInfo.center.x);
						verticies.push_back(-pPos->y * meshloadInfo.scale.y + meshloadInfo.center.y);
						verticies.push_back(pPos->z * meshloadInfo.scale.z + meshloadInfo.center.z);
						break;
					case VERTEX_COMPONENT_NORMAL:
						verticies.push_back(pNormal->x);
						verticies.push_back(-pNormal->y);
						verticies.push_back(pNormal->z);
						break;
					case VERTEX_COMPONENT_UV:
						verticies.push_back(pTexCoord->x * meshloadInfo.uvscale.s);
						verticies.push_back(pTexCoord->y * meshloadInfo.uvscale.t);
						break;
					case VERTEX_COMPONENT_COLOR:
						verticies.push_back(pColor.r);
						verticies.push_back(pColor.g);
						verticies.push_back(pColor.b);
						break;
					case VERTEX_COMPONENT_TANGENT:
						verticies.push_back(pTangent->x);
						verticies.push_back(pTangent->y);
						verticies.push_back(pTangent->z);
						break;
					case VERTEX_COMPONENT_BITANGENT:
						verticies.push_back(pBiTangent->x);
						verticies.push_back(pBiTangent->y);
						verticies.push_back(pBiTangent->z);
						break;
						// Dummy components for padding
					case VERTEX_COMPONENT_DUMMY_FLOAT:
						verticies.push_back(0.0f);
						break;
					case VERTEX_COMPONENT_DUMMY_VEC4:
						verticies.push_back(0.0f);
						verticies.push_back(0.0f);
						verticies.push_back(0.0f);
						verticies.push_back(0.0f);
						break;
					};
				}
			}

			// load indicies
			uint32_t indexBase = static_cast<uint32_t>(indicies.size());
			for (unsigned int j = 0; j < paiMesh->mNumFaces; j++)
			{
				const aiFace& Face = paiMesh->mFaces[j];
				if (Face.mNumIndices != 3)
					continue;
				indicies.push_back(indexBase + Face.mIndices[0]);
				indicies.push_back(indexBase + Face.mIndices[1]);
				indicies.push_back(indexBase + Face.mIndices[2]);
				mesh.parts[i].indexCount += 3;
				indexCount += 3;
			}
		}

		mesh.indexCount = static_cast<uint32_t>(indicies.size());
		mesh.vertexCount = static_cast<uint32_t>((verticies.size() * sizeof(float)) / meshloadInfo.layout.stride());
	}
}