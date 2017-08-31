#include <glm/gtc/matrix_transform.hpp>

#include "defs.h"
#include "debug.h"
#include "camera.h"
#include "utilities.h"

Camera::Camera() {
	aspect_ = 1.3f;
	near_ = 1.f;
	far_ = 100.f;
	fieldOfView_ = 3.141592f / 3.f;

	proj_ = glm::perspective(fieldOfView_, aspect_, near_, far_);

	phi_ = glm::radians(-90.f);
	theta_ = glm::radians(90.f);
}

Camera::~Camera() {
}

void Camera::LookAt(const glm::vec3& eye, const glm::vec3& center) {
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
	
	pos_ += fwd * delta;
}

void Camera::Rotate(const glm::vec2& delta) {
	phi_ += delta.x;
	theta_ += delta.y;
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

	pos_ += right * delta.x;
	pos_ += up * delta.y;
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
