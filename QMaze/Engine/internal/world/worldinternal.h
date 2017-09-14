#pragma once
#include <set>

#include "world.h"
#include "camera.h"
#include "sprite.h"
#include "internal/base/objectinternal.h"

class WorldInternal : public ObjectInternal, public IWorld {
	DEFINE_FACTORY_METHOD(World)

public:
	WorldInternal();

public:
	virtual void Update();
	virtual Object Create(const std::string& type);
	virtual bool CollectSprites(std::vector<Sprite>* sprites, float fieldOfView, float aspect, float nearClipPlane, float farClipPlane);

private:
	struct SpriteComparer {
		bool operator () (Sprite lhs, Sprite rhs) const;
	};

	typedef std::set<Sprite, SpriteComparer> SpriteContainer;

	SpriteContainer sprites_;
};
