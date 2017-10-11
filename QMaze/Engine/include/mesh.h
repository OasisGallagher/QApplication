#pragma once

#include "object.h"
#include "material.h"

struct MaterialTextures {
	Texture bump;
	Texture albedo;
	Texture specular;
};

class IMesh : virtual public IObject {
public:
	virtual void SetMaterialTextures(const MaterialTextures& value) = 0;
	virtual MaterialTextures& GetMaterialTextures() = 0;

	virtual void SetTriangles(unsigned vertexCount, unsigned baseVertex, unsigned baseIndex) = 0;
	virtual void GetTriangles(unsigned& vertexCount, unsigned& baseVertex, unsigned& baseIndex) = 0;
};

typedef std::shared_ptr<IMesh> Mesh;
