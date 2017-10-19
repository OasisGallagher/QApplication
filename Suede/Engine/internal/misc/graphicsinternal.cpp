#include "graphicsinternal.h"
#include "internal/memory/memory.h"
#include "internal/world/worldinternal.h"

Graphics graphicsInstance(Memory::Create<GraphicsInternal>());

void GraphicsInternal::Blit(RenderTexture src, RenderTexture dest, Renderer renderer) {
	Surface surface = dsp_cast<Surface>(worldInstance->Create(ObjectTypeSurface));
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
