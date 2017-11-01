#include "variables.h"
#include "tools/mathf.h"
#include "particlesysteminternal.h"
#include "internal/base/materialinternal.h"
#include "internal/base/rendererinternal.h"
#include "internal/base/transformfeedback.h"

ParticleSystemInternal::ParticleSystemInternal()
	: SpriteInternal(ObjectTypeParticleSystem), currentTfb_(1) {
	tfb_ = Memory::Create<TransformFeedback>();
	InitializeShader();
}

ParticleSystemInternal::~ParticleSystemInternal() {
	Memory::Release(tfb_);
}

void ParticleSystemInternal::SetMaxParticles(int value) {
	maxParticles_ = value;
	particles_.resize(value);
	tfb_->Create(2, maxParticles_ * sizeof(Particle), &particles_[0]);

	Start();
}

void ParticleSystemInternal::Update() {
	int currentVbo = 1 - currentTfb_;
	tfb_->Bind(currentTfb_, currentVbo);

	tfb_->SetVertexDataSource(currentVbo, 0, 1, GL_FLOAT, false, sizeof(Particle), 0);
	tfb_->SetVertexDataSource(currentVbo, 1, 3, GL_FLOAT, false, sizeof(Particle), 4);
	tfb_->SetVertexDataSource(currentVbo, 2, 3, GL_FLOAT, false, sizeof(Particle), 8);
	tfb_->SetVertexDataSource(currentVbo, 3, 1, GL_FLOAT, false, sizeof(Particle), 20);

	UpdateParticles();
	currentTfb_ = 1 - currentTfb_;
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
	update_->Load("buildin/shaders/particle_update");
	billboard_->Load("buildin/shaders/particle_billboard");

	const char* varyings[] = {
		VARIABLE_NAME(varyingType),
		VARIABLE_NAME(varyingPosition),
		VARIABLE_NAME(varyingVelocity),
		VARIABLE_NAME(varyingAge),
	};

	glTransformFeedbackVaryings(update_->GetNativePointer(), CountOf(varyings), varyings, GL_INTERLEAVED_ATTRIBS);
}

void ParticleSystemInternal::InitializeRenderer() {
	Renderer renderer = CREATE_OBJECT(SurfaceRenderer);

	Material material = CREATE_OBJECT(Material);
}
