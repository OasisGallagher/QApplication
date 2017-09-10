#pragma once
#include "world.h"
#include "internal/base/objectinternal.h"

class WorldInternal : public ObjectInternal, public World {
	DEFINE_FACTORY_METHOD(World)

public:
	WorldInternal() : ObjectInternal(ObjectTypeWorld) {
	}

public:
	virtual void Render();
	virtual Camera* AddCamera();
	virtual Sprite* AddSprite();
};
