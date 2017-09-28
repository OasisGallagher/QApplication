#pragma once
#include <set>

#include "world.h"
#include "light.h"
#include "camera.h"
#include "sprite.h"
#include "environment.h"
#include "internal/base/objectinternal.h"

class WorldInternal : public ObjectInternal, public IWorld {
	DEFINE_FACTORY_METHOD(World)

public:
	WorldInternal();

public:
	virtual void Update();
	virtual Object Create(ObjectType type);

	virtual bool GetSprites(ObjectType type, std::vector<Sprite>& sprites);
	virtual Environment GetEnvironment() { return environment_; }

private:
	struct LightComparer { bool operator() (const Light& lhs, const Light& rhs) const; };
	struct SpriteComparer { bool operator() (const Sprite& lhs, const Sprite& rhs) const; };
	struct CameraComparer { bool operator() (const Camera& lhs, const Camera& rhs) const; };

	typedef std::vector<Sprite> SpriteContainer;
	typedef std::set<Light, LightComparer> LightContainer;
	typedef std::set<Camera, CameraComparer> CameraContainer;

private:
	LightContainer lights_;
	SpriteContainer sprites_;
	CameraContainer cameras_;

	Environment environment_;
};
