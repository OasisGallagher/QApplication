#include "particlesystem.h"
#include "internal/sprites/spriteinternal.h"

class ParticleSystemInternal : public IParticleSystem, public SpriteInternal {
	DEFINE_FACTORY_METHOD(ParticleSystem)

public:
	ParticleSystemInternal() : SpriteInternal(ObjectTypeParticleSystem) {}
};
