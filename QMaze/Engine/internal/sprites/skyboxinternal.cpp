#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "camera.h"
#include "skybox.h"
#include "shader.h"
#include "surface.h"
#include "texture.h"
#include "internal/misc/loader.h"
#include "internal/memory/factory.h"
#include "internal/base/renderstate.h"
#include "internal/base/textureinternal.h"
#include "internal/base/surfaceinternal.h"
#include "internal/sprites/skyboxinternal.h"

SkyboxInternal::SkyboxInternal(Camera camera, std::string* textures) {
	camera_ = camera;
	shader_ = Factory::Create<ShaderInternal>();
	shader_->Load("shaders/skybox.glsl");
	shader_->Link();

	texture_ = Factory::Create<Texture3DInternal>();
	texture_->Load(textures);

	mesh_ = Factory::Create<SurfaceInternal>();
	mesh_->Load("models/sphere.obj");
}

SkyboxInternal::~SkyboxInternal() {
}

void SkyboxInternal::Render() {
	shader_->Bind();
	
	RenderState::PushCullFaceEnabled(GL_TRUE);
	RenderState::PushCullFaceFunc(GL_FRONT);

	RenderState::PushDepthTestEnabled(GL_TRUE);
	RenderState::PushDepthTestFunc(GL_LEQUAL);

	glm::mat4 m(1);
	glm::mat4 mvp = camera_->GetProjMatrix() * camera_->GetViewMatrix() * m;
	shader_->SetUniform("MVP", &mvp);

	texture_->Bind(GL_TEXTURE0);
	shader_->SetUniform("textureSampler", 0);

	//mesh_->Render();

	RenderState::PopCullFaceEnabled();
	RenderState::PopCullFaceFunc();
	RenderState::PopDepthTestEnabled();
	RenderState::PopDepthTestFunc();

	texture_->Unbind();
}

Texture3D SkyboxInternal::GetTexture() {
	return texture_;
}
