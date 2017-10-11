#include "engine.h"
#include "graphicsinternal.h"

void GraphicsInternal::Blit(RenderTexture src, RenderTexture dest, Renderer renderer) {
	World world = Engine::get()->world();
	Surface surface = dsp_cast<Surface>(world->Create(ObjectTypeSurface));
	surface->Load("buildin/models/quad.obj");

	for (int i = 0; i < renderer->GetMaterialCount(); ++i) {
		Material material = renderer->GetMaterial(i);
		Mesh mesh = surface->GetMesh(0);
		MaterialTextures& textures = mesh->GetMaterialTextures();
		textures.albedo = src;
	}

	renderer->SetRenderState(Cull, Off);
	renderer->SetRenderState(DepthWrite, Off);
	renderer->SetRenderState(DepthTest, Always);

	renderer->Render(surface);
}
