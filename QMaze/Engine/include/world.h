#pragma once
#include "object.h"

class ICamera;
class ISprite;

class ENGINE_EXPORT IWorld : virtual public IObject {
public:
	virtual void Update() = 0;
	/*
	virtual Camera AddCamera() = 0;
	virtual Sprite AddSprite() = 0;
	*/
};

typedef smart_ptr<IWorld> World;
