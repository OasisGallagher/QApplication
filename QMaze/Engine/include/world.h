#pragma once
#include "object.h"

class Camera;
class Sprite;

class ENGINE_EXPORT World : virtual public Object {
public:
	virtual void Update() = 0;

	virtual Camera* AddCamera() = 0;
	virtual Sprite* AddSprite() = 0;
};
