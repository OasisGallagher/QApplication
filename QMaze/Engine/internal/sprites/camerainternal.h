#pragma once
#include <glm/glm.hpp>

#include "camera.h"
#include "internal/base/objectinternal.h"
#include "internal/sprites/spriteinternal.h"

class IRenderTarget;

class CameraInternal : public ICamera, public SpriteInternal {
	DEFINE_FACTORY_METHOD(Camera)

public:
	CameraInternal();
	~CameraInternal();

public:
	virtual void SetColorType(ClearType type) {}
	virtual void SetClearColor(const glm::vec3& color) {}

	virtual void Render(){}

	virtual void Zoom(const float delta);
	virtual void Move(const glm::vec2& delta);
	virtual void Rotate(const glm::vec2& delta);
	virtual void LookAt(const glm::vec3& eye, const glm::vec3& center);

public:
	virtual void SetAspect(float value) { aspect_ = value; }
	virtual void SetNearClipPlane(float value) { near_ = value; }
	virtual void SetFarClipPlane(float value) { far_ = value; }
	virtual void SetFieldOfView(float value) { near_ = value; }

	virtual float GetAspect() const { return aspect_; }
	virtual float GetNearClipPlane() const { return near_; }
	virtual float GetFarClipPlane() const { return far_; }
	virtual float GetFieldOfView() const {return fieldOfView_;}

	virtual const glm::vec3& GetPosition();
	virtual const glm::mat4& GetProjMatrix();
	virtual const glm::mat4& GetViewMatrix();

private:
	glm::vec3 pos_, center_;
	float phi_, theta_;
	float aspect_;
	float near_, far_;
	float fieldOfView_;

	glm::mat4 proj_, view_;
};
