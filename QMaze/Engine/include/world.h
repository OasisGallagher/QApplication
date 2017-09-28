#pragma once
#include <vector>
#include "sprite.h"
#include "object.h"
#include "environment.h"

class ICamera;
class ISprite;

class ENGINE_EXPORT IWorld : virtual public IObject {
public:
	virtual void Update() = 0;
	virtual Object Create(ObjectType type) = 0;

	virtual bool GetSprites(ObjectType type, std::vector<Sprite>& sprites) = 0;

	virtual Environment GetEnvironment() = 0;
};

typedef smart_ptr<IWorld> World;
