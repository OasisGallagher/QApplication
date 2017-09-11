#include "object.h"
#include "material.h"

class Mesh : virtual public Object {
public:
	virtual void SetMaterial(Material* material) = 0;
	virtual void SetTriangles(unsigned vertexCount, unsigned baseVertex, unsigned baseIndex) = 0;
};
