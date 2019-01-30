#include "Mesh.h"

namespace example {
	void Mesh::loadMesh(glm::vec1 * pVerticies, uint32_t verticiesCount, uint32_t * pIndicies, uint32_t indiciesCount, VertexComponent components, vkw::Buffer & vertexStaging, vkw::Buffer & indexStaging)
	{
		size_t vertexSize = verticiesCount * sizeof(*pVerticies);
		size_t indexSize = indiciesCount * sizeof(*pIndicies);

		vkw::SubBuffer vertStagingBuf = vertexStaging.createSubBuffer(vertexSize);
		vertex
		vkw::SubBuffer indexStaginBuf = indexStaging.createSubBuffer(indexSize);
	}

}
