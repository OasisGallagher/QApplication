#include <vector>

#include "shader.h"
#include "particlesystem.h"
#include "internal/sprites/spriteinternal.h"

class ParticleEmitterInternal : virtual public IParticleEmitter, public ObjectInternal {
public:
	ParticleEmitterInternal(ObjectType type) : ObjectInternal(type), rate_(1) {}

	virtual void SetRate(unsigned value) { rate_ = value; }
	virtual unsigned GetRate() { return rate_; }

	virtual void AddBurst(const ParticleBurst& value) { bursts_.push_back(value); }
	virtual void SetBurst(int i, const ParticleBurst& value) { bursts_[i] = value; }
	virtual ParticleBurst GetBurst(int i) { return bursts_[i]; }
	virtual void RemoveBurst(int i) { bursts_.erase(bursts_.begin() + i); }
	virtual int GetBurstCount() { return bursts_.size(); }

private:
	unsigned rate_;
	std::vector<ParticleBurst> bursts_;
};

class SphereParticleEmitterInternal : public ISphereParticleEmitter, public ParticleEmitterInternal {
public:
	SphereParticleEmitterInternal() : ParticleEmitterInternal(ObjectTypeSphereParticleEmitter) {}

public:
	virtual void SetRadius(float value) { radius_ = value; }
	virtual float GetRadius() { return radius_; }

private:
	float radius_;
};

class ParticleAnimatorInternal : public IParticleAnimator, public ObjectInternal {
public:
	virtual void SetForce(const glm::vec3& value) { force_ = value; }
	virtual glm::vec3 GetForce() { return force_; }

	virtual void SetRandomForce(const glm::vec3& value) { randomForce_ = value; }
	virtual glm::vec3 GetRandomForce() { return randomForce_; }

private:
	glm::vec3 force_;
	glm::vec3 randomForce_;
};

class ParticleSystemInternal : public IParticleSystem, public SpriteInternal {
	DEFINE_FACTORY_METHOD(ParticleSystem)

	struct Particle {
		float size;
		float rotation;

		glm::vec3 color;
		glm::vec3 position;
		glm::vec3 velocity;
	};

public:
	ParticleSystemInternal();
	~ParticleSystemInternal();

public:
	virtual void SetMaxParticles(unsigned value);
	virtual unsigned GetMaxParticles() { return maxParticles_; }

	virtual void SetDuration(float value) { duration_ = value; }
	virtual float GetDuration() { return duration_; }

	virtual void SetLooping(bool value) { looping_ = value; }
	virtual bool GetLooping() { return looping_; }

	virtual void SetStartDelay(float value) { startDelay_ = value; }
	virtual float GetStartDelay() { return startDelay_; }

	virtual void SetStartSpeed(float value) { startSpeed_ = value; }
	virtual float GetStartSpeed() { return startSpeed_; }

	virtual void SetStartColor(const glm::vec3 & value) { startColor_ = value; }
	virtual glm::vec3 GetStartColor() { return startColor_; }

	virtual void SetGravityScale(float value) { gravityScale_ = value; }
	virtual float GetGravityScale() { return gravityScale_; }

	virtual void SetEmitter(ParticleEmitter value) { emitter_ = value; }
	virtual ParticleEmitter GetEmitter() { return emitter_; }

public:
	virtual void Update();

private:
	void Start();

	void InitializeShader();
	void InitializeRenderer();

	void UpdateParticles();

private:
	bool looping_;
	int maxParticles_;
	float duration_;
	float startDelay_;
	float startSpeed_;
	float gravityScale_;
	glm::vec3 startColor_;
	ParticleEmitter emitter_;

	std::vector<Particle> particles_;
};

