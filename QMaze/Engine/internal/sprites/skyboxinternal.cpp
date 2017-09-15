#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "camera.h"
#include "shader.h"
#include "texture.h"
#include "surface.h"
#include "internal/misc/loader.h"
#include "internal/memory/factory.h"
#include "internal/base/textureinternal.h"
#include "internal/base/surfaceinternal.h"
#include "internal/sprites/skyboxinternal.h"

SkyboxInternal::SkyboxInternal() : SpriteInternal(ObjectTypeSkybox) {
}

bool SkyboxInternal::Load(const std::string(&textures)[6]) {
	Texture3D texture = dynamic_sp_cast<Texture3D>(Factory::Create<Texture3DInternal>());
	if (!texture->Load(textures)) {
		return false;
	}

	Surface surface = Factory::Create<SurfaceInternal>();
	if (!surface->Load("buildin/models/box.obj")) {
		return false;
	}

	Shader shader = dynamic_sp_cast<Shader>(Factory::Create("Shader"));
	if (!shader->Load("buildin/shaders/skybox")) {
		return false;
	}

	MaterialTextures materialTextures = surface->GetMesh(0)->GetMaterialTextures();
	materialTextures.diffuse = texture;
	surface->GetMesh(0)->SetMaterialTextures(materialTextures);

	SetSurface(surface);

	Renderer renderer = dynamic_sp_cast<Renderer>(Factory::Create("Renderer"));
	renderer->AddOption(RC_Cull, Front);
	renderer->AddOption(RC_DepthTest, LessEqual);

	Material material = dynamic_sp_cast<Material>(Factory::Create("Material"));
	material->SetShader(shader);
	renderer->AddMaterial(material);

	SetRenderer(renderer);

	return true;
}
