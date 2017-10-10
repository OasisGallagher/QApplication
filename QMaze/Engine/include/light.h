#pragma once

#include <glm/glm.hpp>

#include "sprite.h"

enum LightImportance {
	NotImportant,
	Important,
};

class ILight : virtual public ISprite {
public:
	virtual void SetImportance(LightImportance value) = 0;
	virtual LightImportance GetImportance() const = 0;

	virtual void SetColor(const glm::vec3& value) = 0;
	virtual glm::vec3 GetColor() const = 0;
};

class ISpotLight : virtual public ILight {
};

class IPointLight : virtual public ILight {
public:
	virtual void SetConstant(float value) = 0;
	virtual float GetConstant() = 0;

	virtual void SetLinear(float value) = 0;
	virtual float GetLinear() = 0;

	virtual void SetExp(float value) = 0;
	virtual float GetExp() = 0;
};

class IDirectionalLight : virtual public ILight {
};

typedef smart_ptr<ILight> Light;
typedef smart_ptr<ISpotLight> SpotLight;
typedef smart_ptr<IPointLight> PointLight;
typedef smart_ptr<IDirectionalLight> DirectionalLight;
