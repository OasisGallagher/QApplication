#include "variables.h"
#include "tools/mathf.h"
#include "particlesysteminternal.h"
#include "internal/misc/timefinternal.h"
#include "internal/base/shaderinternal.h"
#include "internal/base/textureinternal.h"
#include "internal/base/surfaceinternal.h"
#include "internal/base/materialinternal.h"
#include "internal/base/rendererinternal.h"

static const int quadIndexes[] = { 0, 1, 2, 3 };
static const glm::vec3 quadVertices[] = {
	glm::vec3(-0.5f, -0.5f, 0.f),
	glm::vec3(0.5f, -0.5f, 0.f),
	glm::vec3(-0.5f,  0.5f, 0.f),
	glm::vec3(0.5f,  0.5f, 0.f),
};

ParticleSystemInternal::ParticleSystemInternal()
	: SpriteInternal(ObjectTypeParticleSystem), duration_(3), looping_(false)
	, startDelay_(0), time_(0), gravityScale_(1), activeParticles_(0), maxParticles_(1000) {
}

ParticleSystemInternal::~ParticleSystemInternal() {
}

void ParticleSystemInternal::SetMaxParticles(unsigned value) {
	maxParticles_ = value;
	colors_.resize(value);
	positions_.resize(value);
	particles_.resize(value);

	InitializeSurface();
	InitializeRenderer();
}

void ParticleSystemInternal::Update() {
	UpdateParticles();

	if (emitter_ && time_ >= startDelay_) {
		UpdateEmitter();
	}

	time_ += timeInstance->GetDeltaTime();
}

void ParticleSystemInternal::UpdateParticles() {
	// TODO: suitable container.
	for (int i = (int)particles_.size() - 1; i >= 0; --i) {
		Particle& particle = particles_[i];
		if (particle.life <= 0) {
			continue;
		}

		float deltaTime = timeInstance->GetDeltaTime();
		if ((particle.life -= deltaTime) > 0) {
			*particle.position += particle.velocity * deltaTime;
			particle.velocity -= 9.8f * deltaTime;
		}
		else {
			--activeParticles_;
		}
	}

	Surface surface = GetSurface();
	surface->UpdateUserBuffer(0, maxParticles_ * sizeof(glm::vec4), &colors_[0]);
	surface->UpdateUserBuffer(1, maxParticles_ * sizeof(glm::vec4), &positions_[0]);
}

void ParticleSystemInternal::UpdateEmitter() {
	unsigned count = Mathf::Max(0, int(GetMaxParticles() - GetParticlesCount()));
	if (count == 0) {
		return;
	}

	unsigned desired = 0;
	emitter_->Emit(nullptr, desired);
	count = Mathf::Min(desired, count);

	if (count != 0) {
		EmitParticles(count);
	}
}

void ParticleSystemInternal::EmitParticles(unsigned count) {
	std::vector<Particle*> cont(count);
	FindUnusedParticles(&cont[0], count);

	int pos = activeParticles_;
	for (int i = 0; i < count; ++i) {
		cont[i]->color = &colors_[pos + i];
		cont[i]->size = &positions_[pos + i].w;
		cont[i]->position = (glm::vec3*)&positions_[pos + i];
	}

	emitter_->Emit(&cont[0], count);
	for (int i = 0; i < count; ++i) {
		*cont[i]->position += GetPosition();
	}

	activeParticles_ += count;
}

void ParticleSystemInternal::InitializeSurface() {
	Mesh mesh = CREATE_OBJECT(Mesh);
	Surface surface = CREATE_OBJECT(Surface);
	mesh->SetPrimaryType(PrimaryTypeTriangleStripe);

	Texture2D albedo = CREATE_OBJECT(Texture2D);
	albedo->Load("textures/snowflake.png");
	mesh->GetMaterialTextures().albedo = albedo;

	SurfaceAttribute attribute;
	attribute.indexes.assign(quadIndexes, quadIndexes + CountOf(quadIndexes));
	// vertices.
	attribute.positions.assign(quadVertices, quadVertices + CountOf(quadVertices));

	// positions.
	attribute.user0.resize(maxParticles_);

	// colors.
	attribute.user1.resize(maxParticles_);

	surface->SetAttribute(attribute);
	mesh->SetTriangles(CountOf(quadVertices), 0, 0);

	surface->AddMesh(mesh);
	SetSurface(surface);
}

void ParticleSystemInternal::InitializeRenderer() {
	ParticleRenderer renderer = CREATE_OBJECT(ParticleRenderer);

	Material material = CREATE_OBJECT(Material);
	Shader shader = CREATE_OBJECT(Shader);
	shader->Load("buildin/shaders/particle");
	material->SetShader(shader);

	renderer->AddMaterial(material);
	SetRenderer(renderer);
}

void ParticleSystemInternal::FindUnusedParticles(Particle** container, unsigned count) {
	for (int i = 0, j = 0; i < particles_.size() && j < count; ++i) {
		if (particles_[i].life <= 0) {
			container[j++] = &particles_[i];
		}
	}
}

ParticleEmitterInternal::ParticleEmitterInternal(ObjectType type) : ObjectInternal(type)
	, rate_(1), time_(-1), remainder_(0) {
}

void ParticleEmitterInternal::Emit(Particle** particles, unsigned& count) {
	if (particles == nullptr) {
		count = CalculateNextEmissionParticleCount();
		return;
	}

	if (time_ < 0) { time_ = 0; }

	EmitParticles(particles, count);
}

void ParticleEmitterInternal::EmitParticles(Particle** particles, unsigned count) {
	for (unsigned i = 0; i < count; ++i) {
		Particle* item = particles[i];
		item->life = GetStartDuration();
		*item->size = GetStartSize();
		item->velocity = GetStartVelocity();

		*item->color = GetStartColor();
		*item->position = GetStartPosition();
	}
}

unsigned ParticleEmitterInternal::CalculateNextEmissionParticleCount() {
	unsigned ans = rate_;
	float nextTime = time_ + timeInstance->GetDeltaTime();
	for (int i = 0; i < bursts_.size(); ++i) {
		if (bursts_[i].time > time_ && bursts_[i].time <= nextTime) {
			ans = Mathf::Random(bursts_[i].min, bursts_[i].max);
		}
	}

	remainder_ += ans * timeInstance->GetDeltaTime();
	ans = (unsigned)remainder_;
	remainder_ -= ans;
	return ans;
}

void ParticleAnimatorInternal::Animate(Particle& particle) {

}
