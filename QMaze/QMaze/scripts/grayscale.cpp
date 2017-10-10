#include "engine.h"
#include "graphics.h"
#include "renderer.h"
#include "grayscale.h"

void Grayscale::OnRenderImage(RenderTexture src, RenderTexture dest) {
	World world = Engine::get()->world();
	Renderer renderer = dynamic_sp_cast<Renderer>(world->Create(ObjectTypeRenderer));
	
	Shader shader = dynamic_sp_cast<Shader>(world->Create(ObjectTypeShader));
	shader->Load("buildin/shaders/blit");
	
	Material material = dynamic_sp_cast<Material>(world->Create(ObjectTypeMaterial));
	material->SetShader(shader);

	renderer->AddMaterial(material);
	Graphics::Blit(src, dest, renderer);
}
