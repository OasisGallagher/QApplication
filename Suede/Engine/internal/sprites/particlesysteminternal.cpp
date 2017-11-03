#include "variables.h"
#include "tools/mathf.h"
#include "particlesysteminternal.h"
#include "internal/base/materialinternal.h"
#include "internal/base/rendererinternal.h"

ParticleSystemInternal::ParticleSystemInternal()
	: SpriteInternal(ObjectTypeParticleSystem), duration_(3), looping_(false)
	, startDelay_(0), startSpeed_(1), gravityScale_(1) {
	InitializeShader();
}

ParticleSystemInternal::~ParticleSystemInternal() {
}

void ParticleSystemInternal::SetMaxParticles(unsigned value) {
	maxParticles_ = value;
	particles_.resize(value);

	Start();
}

void ParticleSystemInternal::Update() {
	UpdateParticles();
}

void ParticleSystemInternal::UpdateParticles() {

}

void ParticleSystemInternal::Start() {
	memset(&particles_[0], 0, particles_.size() * sizeof(Particle));

	particles_[0].type = 0;
	particles_[0].position = GetPosition();
	particles_[0].velocity = glm::vec3(0, 0.0001f, 0);
	particles_[0].lifeTime = 0;
}

void ParticleSystemInternal::InitializeShader() {
}

void ParticleSystemInternal::InitializeRenderer() {
	Renderer renderer = CREATE_OBJECT(SurfaceRenderer);

	Material material = CREATE_OBJECT(Material);
}
