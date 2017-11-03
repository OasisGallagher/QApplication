#pragma once
#include "sprite.h"

struct ParticleBurst {
	float time;
	unsigned min;
	unsigned max;
};

class IParticleEmitter;
class ISphereParticleEmitter;
class IParticleAnimator;

typedef std::shared_ptr<IParticleEmitter> ParticleEmitter;
typedef std::shared_ptr<ISphereParticleEmitter> SphereParticleEmitter;
typedef std::shared_ptr<IParticleAnimator> ParticleAnimator;

class IParticleEmitter : virtual public IObject {
public:
	virtual void SetRate(unsigned value) = 0;
	virtual unsigned GetRate() = 0;

	virtual void AddBurst(const ParticleBurst& value) = 0;
	virtual void SetBurst(int i, const ParticleBurst& value) = 0;
	virtual ParticleBurst GetBurst(int i) = 0;
	virtual void RemoveBurst(int i) = 0;
	virtual int GetBurstCount() = 0;
};

class ISphereParticleEmitter : virtual public IParticleEmitter {
public:
	virtual void SetRadius(float value) = 0;
	virtual float GetRadius() = 0;
};

class IParticleAnimator : virtual public IObject {
public:
	virtual void SetForce(const glm::vec3& value) = 0;
	virtual glm::vec3 GetForce() = 0;

	virtual void SetRandomForce(const glm::vec3& value) = 0;
	virtual glm::vec3 GetRandomForce() = 0;
};

class ENGINE_EXPORT IParticleSystem : virtual public ISprite {
public:
	virtual void SetDuration(float value) = 0;
	virtual float GetDuration() = 0;

	virtual void SetLooping(bool value) = 0;
	virtual bool GetLooping() = 0;

	virtual void SetStartDelay(float value) = 0;
	virtual float GetStartDelay() = 0;

	virtual void SetStartSpeed(float value) = 0;
	virtual float GetStartSpeed() = 0;

	virtual void SetStartColor(const glm::vec3& value) = 0;
	virtual glm::vec3 GetStartColor() = 0;

	virtual void SetGravityScale(float value) = 0;
	virtual float GetGravityScale() = 0;

	virtual void SetMaxParticles(unsigned value) = 0;
	virtual unsigned GetMaxParticles() = 0;

	virtual void SetEmitter(ParticleEmitter value) = 0;
	virtual ParticleEmitter GetEmitter() = 0;
};

typedef std::shared_ptr<IParticleSystem> ParticleSystem;
