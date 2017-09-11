#pragma once
#include <string>
#include "defines.h"

enum ObjectType {
	ObjectSkybox,
	ObjectShader,
	ObjectTexture2D,
	ObjectTexture3D,
	ObjectTypeWorld,
	ObjectTypeCamera,
	ObjectTypeMaterial,
	ObjectTypeSurface,
	ObjectTypeSprite,
	ObjectTypeCount,
};

class ENGINE_EXPORT Object {
public:
	virtual ~Object() {}
	virtual unsigned GetInstanceID() const = 0;
	virtual int GetType() const = 0;

	virtual std::string GetName() const = 0;
	virtual void SetName(const std::string& value) = 0;
};
