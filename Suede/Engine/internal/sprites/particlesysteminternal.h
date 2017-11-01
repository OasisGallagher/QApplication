#include <vector>

#include "shader.h"
#include "particlesystem.h"
#include "internal/sprites/spriteinternal.h"

class TransformFeedback;

class ParticleSystemInternal : public IParticleSystem, public SpriteInternal {
	DEFINE_FACTORY_METHOD(ParticleSystem)

	struct Particle {
		int type;
		float lifeTime;
		glm::vec3 position;
		glm::vec3 velocity;
	};

public:
	ParticleSystemInternal();
	~ParticleSystemInternal();

public:
	virtual void SetMaxParticles(int value);
	virtual int GetMaxParticles() { return maxParticles_; }
	virtual void Update();

private:
	void Start();

	void InitializeShader();
	void InitializeRenderer();

	void UpdateParticles();

private:
	Shader update_;
	Shader billboard_;

	TransformFeedback* tfb_;

	int maxParticles_;
	std::vector<Particle> particles_;

	int currentTfb_;
};
