#pragma once

#include "mesh.h"
#include "internal/base/objectinternal.h"

class MeshInternal : public IMesh, public ObjectInternal {
	DEFINE_FACTORY_METHOD(Mesh)

public:
	MeshInternal() :ObjectInternal(ObjectTypeMesh) {}

	virtual void SetMaterial(Material material) {}
	virtual void SetTriangles(unsigned vertexCount, unsigned baseVertex, unsigned baseIndex) {}
};
