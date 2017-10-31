#include "particlesystem.h"
#include "internal/sprites/spriteinternal.h"

class ParticleSystemInternal : public IParticleSystem, public SpriteInternal {
	DEFINE_FACTORY_METHOD(ParticleSystem)

	struct Particle {
		int type;
		float duration;
		glm::vec3 position;
		glm::vec3 velocity;
	};

public:
	ParticleSystemInternal() : SpriteInternal(ObjectTypeParticleSystem) {}
};
