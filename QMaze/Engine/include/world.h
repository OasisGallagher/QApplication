#pragma once
#include <vector>
#include "sprite.h"
#include "object.h"

class ICamera;
class ISprite;

class ENGINE_EXPORT IWorld : virtual public IObject {
public:
	virtual void Update() = 0;
	virtual Object Create(const std::string& type) = 0;
	virtual bool CollectSprites(std::vector<Sprite>* sprites, float fieldOfView, float aspect, float nearClipPlane, float farClipPlane) = 0;
};

typedef smart_ptr<IWorld> World;
