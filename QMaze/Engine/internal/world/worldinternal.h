#pragma once
#include <set>

#include "world.h"
#include "light.h"
#include "camera.h"
#include "sprite.h"
#include "internal/base/objectinternal.h"

class WorldInternal : public ObjectInternal, public IWorld {
	DEFINE_FACTORY_METHOD(World)

public:
	WorldInternal();

public:
	virtual void Update();
	virtual Object Create(ObjectType type);

	virtual void GetSprites(ObjectType type, std::vector<Sprite>& sprites);

private:
	struct LightComparer { bool operator() (Light& lhs, Light& rhs) const; };
	struct SpriteComparer { bool operator() (Sprite& lhs, Sprite& rhs) const; };
	struct CameraComparer { bool operator() (Camera& lhs, Camera& rhs) const; };

	typedef std::vector<Sprite> SpriteContainer;
	typedef std::set<Light, LightComparer> LightContainer;
	typedef std::set<Camera, CameraComparer> CameraContainer;

private:
	LightContainer lights_;
	SpriteContainer sprites_;
	CameraContainer cameras_;
};
