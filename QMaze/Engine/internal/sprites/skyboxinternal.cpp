#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "texture.h"
#include "surface.h"
#include "internal/misc/loader.h"
#include "internal/memory/factory.h"
#include "internal/base/textureinternal.h"
#include "internal/base/surfaceinternal.h"
#include "internal/sprites/skyboxinternal.h"

SkyboxInternal::SkyboxInternal(Camera camera, std::string* textures) {
	shader_ = Factory::Create<ShaderInternal>();
	shader_->Load("buildin/shaders/skybox.glsl");
	shader_->Link();

	Texture3D texture = dynamic_sp_cast<Texture3D>(Factory::Create<Texture3DInternal>());
	texture->Load(textures);

	Surface surface = Factory::Create<SurfaceInternal>();
	surface->Load("models/sphere.obj");

	MaterialTextures materialTextures = surface->GetMesh(0)->GetMaterialTextures();
	materialTextures.diffuse = texture;
	surface->GetMesh(0)->SetMaterialTextures(materialTextures);

	SetSurface(surface);
}

SkyboxInternal::~SkyboxInternal() {
}

// void SkyboxInternal::Render() {
// 	shader_->Bind();
// 	
// 	RenderState::PushCullFaceEnabled(GL_TRUE);
// 	RenderState::PushCullFaceFunc(GL_FRONT);
// 
// 	RenderState::PushDepthTestEnabled(GL_TRUE);
// 	RenderState::PushDepthTestFunc(GL_LEQUAL);
// 
// 	glm::mat4 m(1);
// 	glm::mat4 mvp = camera_->GetProjMatrix() * camera_->GetViewMatrix() * m;
// 	shader_->SetUniform("MVP", &mvp);
// 
// 	texture_->Bind(GL_TEXTURE0);
// 	shader_->SetUniform("textureSampler", 0);
// 
// 	//mesh_->Render();
// 
// 	RenderState::PopCullFaceEnabled();
// 	RenderState::PopCullFaceFunc();
// 	RenderState::PopDepthTestEnabled();
// 	RenderState::PopDepthTestFunc();
// 
// 	texture_->Unbind();
// }
// 
// Texture3D SkyboxInternal::GetTexture() {
// 	return texture_;
// }
