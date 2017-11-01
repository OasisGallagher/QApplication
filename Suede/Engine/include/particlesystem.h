#pragma once
#include "sprite.h"

class ENGINE_EXPORT IParticleSystem : virtual public ISprite {
public:
	virtual void SetMaxParticles(int value) = 0;
	virtual int GetMaxParticles() = 0;
};

typedef std::shared_ptr<IParticleSystem> ParticleSystem;
