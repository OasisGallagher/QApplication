#pragma once
#include <memory>
#include <string>
#include "defines.h"

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

class ENGINE_EXPORT IObject : public std::enable_shared_from_this<IObject> {
public:
	virtual ~IObject() {}

public:
	virtual ObjectType GetType() = 0;
	virtual unsigned GetInstanceID() = 0;
};

typedef std::shared_ptr<IObject> Object;

/**
 * dynamic shared_ptr cast.
 */
template<class T, class U>
inline T dsp_cast(const std::shared_ptr<U>& ptr) {
	typename T::element_type* p = dynamic_cast<typename T::element_type*>(ptr.get());
	if (NULL != p) {
		return T(ptr, p);
	}

	return T();
}
