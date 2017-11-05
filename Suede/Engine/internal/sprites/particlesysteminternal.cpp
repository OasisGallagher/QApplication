#include "variables.h"
#include "tools/mathf.h"
#include "particlesysteminternal.h"
#include "internal/misc/timefinternal.h"
#include "internal/base/shaderinternal.h"
#include "internal/base/textureinternal.h"
#include "internal/base/surfaceinternal.h"
#include "internal/base/materialinternal.h"
#include "internal/base/rendererinternal.h"

static const int quadIndexes[] = { 1, 2, 3, 4 };
static const glm::vec3 quadVertices[] = {
	glm::vec3(-0.5f, -0.5f, 0.f),
	glm::vec3(0.5f, -0.5f, 0.f),
	glm::vec3(-0.5f,  0.5f, 0.f),
	glm::vec3(0.5f,  0.5f, 0.f),
};

ParticleSystemInternal::ParticleSystemInternal()
	: SpriteInternal(ObjectTypeParticleSystem), duration_(3), looping_(false)
	, startDelay_(0), time_(0), gravityScale_(1), maxParticles_(1000) {
	InitializeSurface();
	InitializeRenderer();
}

ParticleSystemInternal::~ParticleSystemInternal() {
}

void ParticleSystemInternal::SetMaxParticles(unsigned value) {
	maxParticles_ = value;
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
		if ((particle.life -= timeInstance->GetDeltaTime()) <= 0) {
			particles_.erase(particles_.begin() + i);
		}
	}
}

void ParticleSystemInternal::UpdateEmitter() {
	unsigned maxCount = Mathf::Max(0, int(GetMaxParticles() - GetParticlesCount()));
	if (maxCount > 0) {
		std::vector<Particle> generation(maxCount);
		emitter_->Emit(&generation[0], maxCount);

		particles_.insert(particles_.end(), generation.begin(), generation.begin() + maxCount);
	}
}

void ParticleSystemInternal::InitializeSurface() {
	Mesh mesh = CREATE_OBJECT(Mesh);
	Surface surface = CREATE_OBJECT(Surface);
	mesh->SetPrimaryType(PrimaryTypeTriangleStripe);

	Texture2D albedo = CREATE_OBJECT(Texture2D);
	albedo->Load("textures/fireworks_red.jpg");
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

void ParticleEmitterInternal::Emit(Particle* particles, unsigned& count) {
	if (time_ < 0) { time_ = 0; }
	float nextTime = time_ + timeInstance->GetDeltaTime();
	count = Mathf::Min(count, CalculateEmitParticleCount(time_, nextTime));

	for (unsigned i = 0; i < count; ++i) {
		Particle& item = particles[i];
		item.life = GetStartDuration();
		item.color = GetStartColor();
		item.size = GetStartSize();
		item.velocity = GetStartVelocity();
		item.position = GetStartPosition();
	}
}

unsigned ParticleEmitterInternal::CalculateEmitParticleCount(float current, float next) {
	for (int i = 0; i < bursts_.size(); ++i) {
		if (bursts_[i].time > current && bursts_[i].time <= next) {
			return Mathf::Random(bursts_[i].min, bursts_[i].max);
		}
	}

	return GetRate();
}

void ParticleAnimatorInternal::Animate(Particle& particle) {

}
