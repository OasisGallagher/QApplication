#pragma once

#include "object.h"
#include "material.h"

struct MeshTextures {
	Texture2D normal;
	Texture2D diffuse;
	Texture2D specular;
};

class IMesh : virtual public IObject {
public:
	virtual void SetTextures(const MeshTextures& value) = 0;
	virtual MeshTextures GetTextures() = 0;

	virtual void SetTriangles(unsigned vertexCount, unsigned baseVertex, unsigned baseIndex) = 0;
	virtual void GetTriangles(unsigned& vertexCount, unsigned& baseVertex, unsigned& baseIndex) = 0;
};

typedef smart_ptr<IMesh> Mesh;
