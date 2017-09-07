#pragma once

#include "defs.h"
#include "texture.h"

class MaterialPrivate {
public:
	MaterialPrivate() {}
	~MaterialPrivate() {}

public:
	void SetDiffuseTexture(Texture value) { diffuse_ = value; }
	Texture GetDiffuseTexture() const { return diffuse_; }

private:
	Texture diffuse_;
};

class ENGINE_EXPORT Material : public Object {
	IMPLEMENT_SMART_OBJECT(Material);
};
