#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "camera.h"
#include "skybox.h"
#include "shader.h"
#include "loader.h"
#include "surface.h"
#include "texture.h"
#include "utilities.h"
#include "renderstate.h"

Skybox::Skybox(Camera* camera, std::string* textures) : Object(ObjectSkybox) {
	camera_ = camera;
	shader_ = new Shader;
	shader_->Load("shaders/skybox.glsl");
	shader_->Link();

	texture_ = new Texture3D;
	texture_->Load(textures);

	mesh_ = new Surface;
	mesh_->Load("models/sphere.obj");
}

Skybox::~Skybox() {
	delete mesh_;
	delete shader_;
	delete texture_;
}

void Skybox::Render() {
	shader_->Bind();
	
	RenderState::PushCullFaceEnabled(GL_TRUE);
	RenderState::PushCullFaceFunc(GL_FRONT);

	RenderState::PushDepthTestEnabled(GL_TRUE);
	RenderState::PushDepthTestFunc(GL_LEQUAL);

	glm::mat4 m(1);
	float scale = 20.f;
	m = glm::translate(m, camera_->GetPosition()) * glm::scale(m, glm::vec3(scale)) * rotation_;
	
	glm::mat4 mvp = camera_->GetProjMatrix() * camera_->GetViewMatrix() * m;
	shader_->SetUniform("MVP", &mvp);

	texture_->Bind(GL_TEXTURE0);
	shader_->SetUniform("textureSampler", 0);

	mesh_->Render();

	RenderState::PopCullFaceEnabled();
	RenderState::PopCullFaceFunc();
	RenderState::PopDepthTestEnabled();
	RenderState::PopDepthTestFunc();

	texture_->Unbind();
}

Texture3D* Skybox::GetTexture() {
	return texture_;
}

void Skybox::Rotate(const glm::mat4& rotation) {
	rotation_ = rotation;
}
