#pragma once

#include "object.h"

class Texture;

class ENGINE_EXPORT Material : virtual public Object {
public:
	virtual void SetDiffuseTexture(Texture* value) = 0;
	virtual Texture* GetDiffuseTexture() = 0;
};
