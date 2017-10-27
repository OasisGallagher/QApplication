#pragma once
#include "sprite.h"

class ENGINE_EXPORT IParticleSystem : virtual public ISprite {
public:
};

typedef std::shared_ptr<IParticleSystem> ParticleSystem;
