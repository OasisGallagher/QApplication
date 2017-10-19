#pragma once

#include "mesh.h"
#include "internal/base/objectinternal.h"

class MeshInternal : public IMesh, public ObjectInternal {
	DEFINE_FACTORY_METHOD(Mesh)

public:
	MeshInternal();

	virtual void SetMaterialTextures(const MaterialTextures& value) { textures_ = value; }
	virtual MaterialTextures& GetMaterialTextures() { return textures_; }

	virtual void SetTriangles(unsigned vertexCount, unsigned baseVertex, unsigned baseIndex);
	virtual void GetTriangles(unsigned& vertexCount, unsigned& baseVertex, unsigned& baseIndex);

private:
	MaterialTextures textures_;

	unsigned baseIndex_;
	unsigned baseVertex_;
	unsigned vertexCount_;
};
