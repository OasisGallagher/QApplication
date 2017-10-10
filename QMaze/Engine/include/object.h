#pragma once
#include <string>
#include "defines.h"
#include "smartptr.h"

enum ObjectType {
	ObjectTypeMesh,
	ObjectTypeWorld,
	ObjectTypeShader,
	ObjectTypeSurface,
	ObjectTypeMaterial,
	ObjectTypeRenderer,
	ObjectTypeTexture2D,
	ObjectTypeTextureCube,
	ObjectTypeRenderTexture,

	// Sprites.
	ObjectTypeSprite,
	ObjectTypeCamera,
	ObjectTypeSkybox,
	ObjectTypeSpotLight,
	ObjectTypePointLight,
	ObjectTypeDirectionalLight,

	ObjectTypeCount,

	ObjectTypeLights,
};

class ENGINE_EXPORT IObject {
public:
	virtual ~IObject() {}

public:
	virtual ObjectType GetType() const = 0;
	virtual unsigned GetInstanceID() const = 0;
};

typedef smart_ptr<IObject> Object;
