#pragma once
#include <string>
#include "defines.h"
#include "smartptr.h"

enum ObjectType {
	ObjectSkybox,
	ObjectShader,
	ObjectTypeMesh,
	ObjectTexture2D,
	ObjectTexture3D,
	ObjectTypeWorld,
	ObjectTypeCamera,
	ObjectTypeSprite,
	ObjectTypeSurface,
	ObjectTypeMaterial,
	ObjectTypeRenderer,
	ObjectTypeCount,
};

class ENGINE_EXPORT IObject {
public:
	virtual ~IObject() {}

public:
	virtual unsigned GetInstanceID() const = 0;
	virtual ObjectType GetType() const = 0;

	virtual std::string GetName() const = 0;
	virtual void SetName(const std::string& value) = 0;
};

typedef smart_ptr<IObject> Object;
