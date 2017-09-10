#pragma once

#include "material.h"
#include "internal/base/objectinternal.h"

class MaterialInternal : public Material, public ObjectInternal {
public:
	MaterialInternal() : ObjectInternal(ObjectTypeMaterial) {
	}
	~MaterialInternal() {}

public:
	virtual void SetDiffuseTexture(Texture* value) { diffuse_ = value; }
	virtual Texture* GetDiffuseTexture() { return diffuse_; }

private:
	Texture* diffuse_;
};
