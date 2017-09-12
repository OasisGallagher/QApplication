#pragma once
#include "object.h"
#include "texture.h"

class ENGINE_EXPORT ISkybox : virtual public IObject {
public:
	virtual void Render() = 0;
	virtual ITexture3D* GetTexture() = 0;
};

typedef smart_ptr<ISkybox> Skybox;
