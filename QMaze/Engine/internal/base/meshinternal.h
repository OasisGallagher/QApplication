#include "mesh.h"
#include "internal/base/objectinternal.h"

class MeshInternal : public Mesh, public ObjectInternal {
public:
	virtual void SetVertices(const glm::vec3* value);
	virtual void SetNormals(const glm::vec3* value);
	virtual void SetUVs(const glm::vec3* value);
	virtual void SetIndices(unsigned* indices);

	virtual void SetTriangles(unsigned vertexCount, unsigned baseVertex, unsigned baseIndex);
};
