#pragma once
#include "posteffect.h"
#include "engine.h"

class Inversion : public PostEffect {
public:
	virtual void OnRenderImage(RenderTexture src, RenderTexture dest) {
		World world = Engine::get()->world();
		Renderer renderer = dynamic_sp_cast<Renderer>(world->Create(ObjectTypeRenderer));

		Shader shader = dynamic_sp_cast<Shader>(world->Create(ObjectTypeShader));
		shader->Load("shaders/inversion");

		Material material = dynamic_sp_cast<Material>(world->Create(ObjectTypeMaterial));
		material->SetShader(shader);

		renderer->AddMaterial(material);
		Engine::get()->graphics()->Blit(src, dest, renderer);
	}
};
