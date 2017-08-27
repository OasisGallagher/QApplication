#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

#include "defs.h"
#include "debug.h"
#include "camera.h"
#include "utilities.h"

Camera::Camera() {
	float aspect = (float)Globals::kWindowWidth / Globals::kWindowHeight;
	proj_ = glm::perspective(Globals::kFieldOfView, aspect, Globals::kNearPlane, Globals::kFarPlane);

	zspeed_ = 0.05f;
	mspeed_ = 0.05f;
	rspeed_ = 0.005f;
	phi_ = glm::radians(-90.f);
	theta_ = glm::radians(90.f);
}

Camera::~Camera() {
}

void Camera::Reset(const glm::vec3& eye, const glm::vec3& center) {
	pos_ = eye;

	center_ = center;

	glm::vec3 fwd = glm::normalize(eye - center);

	theta_ = acosf(fwd.y);
	phi_ = atanf(fwd.z / fwd.x);
}

void Camera::Zoom(float delta) {
	if (delta == 0) { return; }
	glm::vec3 fwd(
		sinf(theta_) * cosf(phi_),
		cosf(theta_),
		sinf(theta_) *sin(phi_)
		);
	
	pos_ += fwd * delta * zspeed_;
}

void Camera::Rotate(const glm::vec2& delta) {
	phi_ += rspeed_ * delta.x;
	theta_ += rspeed_ * delta.y;
}

void Camera::Move(const glm::vec2& delta) {
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

	pos_ += right * delta.x * mspeed_;
	pos_ += up * delta.y * mspeed_;
}

const glm::vec3& Camera::GetPosition() {
	return pos_;
}

const glm::mat4& Camera::GetProjMatrix() {
	return proj_;
}

const glm::mat4& Camera::GetViewMatrix() {
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
