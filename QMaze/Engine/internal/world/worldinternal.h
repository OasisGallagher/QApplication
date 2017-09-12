#pragma once
#include "world.h"
#include "camera.h"
#include "sprite.h"
#include "internal/base/objectinternal.h"

class WorldInternal : public ObjectInternal, public IWorld {
	DEFINE_FACTORY_METHOD(World)

public:
	WorldInternal() : ObjectInternal(ObjectTypeWorld) {
	}

public:
	virtual void Update() {}
	virtual Camera AddCamera() { return Camera(); }
	virtual Sprite AddSprite() { return Sprite(); }
};
