#pragma once
#include <set>

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
	virtual void Update();
	virtual Camera AddCamera();
	virtual Sprite AddSprite();

private:
	struct SpriteComparer {
		bool operator () (Sprite lhs, Sprite rhs) const;
	};

	typedef std::set<Sprite, SpriteComparer> SpriteContainer;

	SpriteContainer sprites_;
};
