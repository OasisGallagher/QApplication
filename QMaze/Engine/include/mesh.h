#pragma once

#include "object.h"
#include "material.h"

class IMesh : virtual public IObject {
public:
	virtual void SetMaterial(Material material) = 0;
	virtual void SetTriangles(unsigned vertexCount, unsigned baseVertex, unsigned baseIndex) = 0;
};

typedef smart_ptr<IMesh> Mesh;
