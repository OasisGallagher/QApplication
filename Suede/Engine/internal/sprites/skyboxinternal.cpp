#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "internal/memory/factory.h"
#include "internal/base/shaderinternal.h"
#include "internal/base/textureinternal.h"
#include "internal/base/surfaceinternal.h"
#include "internal/base/rendererinternal.h"
#include "internal/base/materialinternal.h"
#include "internal/sprites/skyboxinternal.h"

SkyboxInternal::SkyboxInternal() : SpriteInternal(ObjectTypeSkybox) {
}

bool SkyboxInternal::Load(const std::string(&textures)[6]) {
	TextureCube texture = Factory::Create<TextureCubeInternal>();
	if (!texture->Load(textures)) {
		return false;
	}

	Surface surface = Factory::Create<SurfaceInternal>();
	if (!surface->Load("buildin/models/box.obj")) {
		return false;
	}

	Shader shader = Factory::Create<ShaderInternal>();
	if (!shader->Load("buildin/shaders/skybox")) {
		return false;
	}

	MaterialTextures& materialTextures = surface->GetMesh(0)->GetMaterialTextures();
	materialTextures.albedo = texture;

	SetSurface(surface);

	Renderer renderer = Factory::Create<RendererInternal>();
	renderer->SetRenderQueue(Background);

	renderer->SetRenderState(Cull, Front);
	renderer->SetRenderState(DepthTest, LessEqual);

	Material material = Factory::Create<MaterialInternal>();
	material->SetShader(shader);
	renderer->AddMaterial(material);

	SetRenderer(renderer);

	return true;
}
