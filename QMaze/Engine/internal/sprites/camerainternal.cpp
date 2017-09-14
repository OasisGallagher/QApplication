#include <glm/gtc/matrix_transform.hpp>

#include "engine.h"
#include "camera.h"
#include "tools/debug.h"
#include "internal/base/variables.h"
#include "internal/resources/resources.h"
#include "internal/base/shaderinternal.h"
#include "internal/base/materialinternal.h"
#include "internal/base/rendererinternal.h"
#include "internal/sprites/camerainternal.h"

CameraInternal::CameraInternal() {
	aspect_ = 1.3f;
	near_ = 1.f;
	far_ = 100.f;
	fieldOfView_ = 3.141592f / 3.f;

	proj_ = glm::perspective(fieldOfView_, aspect_, near_, far_);

	//phi_ = glm::radians(-90.f);
	//theta_ = glm::radians(90.f);
	glClearDepth(1);
}

CameraInternal::~CameraInternal() {
}

void CameraInternal::SetClearColor(const glm::vec3& color) {
	glClearColor(color.r, color.g, color.b, 1);
}

void CameraInternal::Update() {
	World world = Engine::Ptr()->WorldPtr();
	std::vector<Sprite> sprites;
	if (!world->CollectSprites(&sprites, fieldOfView_, aspect_, near_, far_)) {
		return;
	}

	for (int i = 0; i < sprites.size(); ++i) {
		Sprite sprite = sprites[i];
		if (sprite->GetRenderer() && sprite->GetSurface()) {
			RenderSprite(sprite);
		}
	}
}

void CameraInternal::RenderSprite(Sprite sprite) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	Surface surface = sprite->GetSurface();
	Renderer renderer = sprite->GetRenderer();

	renderer->SetSurface(surface);
	Material material = renderer->GetMaterial(0);

	glm::mat4 matrix = proj_ * GetWorldToLocalMatrix() * sprite->GetLocalToWorldMatrix();
	material->SetMatrix(Variables::modelToClipSpaceMatrix, matrix);
	
	renderer->Render();
}
/*
void CameraInternal::LookAt(const glm::vec3& eye, const glm::vec3& center) {
	pos_ = eye;

	center_ = center;

	glm::vec3 fwd = glm::normalize(eye - center);

	theta_ = acosf(fwd.y);
	phi_ = atanf(fwd.z / fwd.x);
}

void CameraInternal::Zoom(float delta) {
	if (delta == 0) { return; }
	glm::vec3 fwd(
		sinf(theta_) * cosf(phi_),
		cosf(theta_),
		sinf(theta_) *sin(phi_)
		);
	
	pos_ += fwd * delta;
}

void CameraInternal::Rotate(const glm::vec2& delta) {
	phi_ += delta.x;
	theta_ += delta.y;
}

void CameraInternal::Move(const glm::vec2& delta) {
	glm::vec3 fwd(
		sinf(theta_) * cosf(phi_),
		cosf(theta_),
		sinf(theta_) *sin(phi_)
	);

	glm::vec3 right(
		cosf(glm::radians(90.f) + phi_),
		0,
		sinf(glm::radians(90.f) + phi_)
	);

	glm::vec3 up = glm::cross(right, fwd);

	pos_ += right * delta.x;
	pos_ += up * delta.y;
}

const glm::vec3& CameraInternal::GetPosition() {
	return pos_;
}
*/
const glm::mat4& CameraInternal::GetProjMatrix() {
	return proj_;
}
/*
const glm::mat4& CameraInternal::GetViewMatrix() {
	//return view_ = glm::lookAt(pos_, center_, glm::vec3(0, 1, 0));

	glm::vec3 fwd(
		sinf(theta_) * cosf(phi_),
		cosf(theta_),
		sinf(theta_) *sin(phi_)
		);

	glm::vec3 right(
		cosf(glm::radians(90.f) + phi_),
		0,
		sinf(glm::radians(90.f) + phi_)
		);

	glm::vec3 up = glm::cross(right, fwd);

	view_= glm::lookAt(pos_, pos_ - fwd, up);
	return view_;
}
*/