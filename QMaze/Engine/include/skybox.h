#pragma once
#include "object.h"
#include "texture.h"

class ENGINE_EXPORT Skybox : virtual public Object {
public:
	virtual void Render() = 0;
	virtual Texture3D* GetTexture() = 0;
};
