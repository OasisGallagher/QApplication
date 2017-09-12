#pragma once
#include <glm/glm.hpp>
#include "sprite.h"

enum ClearType {
	ClearColor,
	ClearSkybox,
};

class ENGINE_EXPORT ICamera : virtual public ISprite {
public:
	virtual void SetColorType(ClearType type) = 0;
	virtual void SetClearColor(const glm::vec3& color) = 0;
	virtual void Render() = 0;

	virtual void Zoom(const float delta) = 0;
	virtual void Move(const glm::vec2& delta) = 0;
	virtual void Rotate(const glm::vec2& delta) = 0;
	virtual void LookAt(const glm::vec3& eye, const glm::vec3& center) = 0;

	virtual void SetAspect(float value) = 0;
	virtual void SetNearClipPlane(float value) = 0;
	virtual void SetFarClipPlane(float value) = 0;
	virtual void SetFieldOfView(float value) = 0;

	virtual float GetAspect() const = 0;
	virtual float GetNearClipPlane() const = 0;
	virtual float GetFarClipPlane() const = 0;
	virtual float GetFieldOfView() const = 0;

	virtual const glm::vec3& GetPosition() = 0;
	virtual const glm::mat4& GetProjMatrix() = 0;
	virtual const glm::mat4& GetViewMatrix() = 0;
};

typedef smart_ptr<ICamera> Camera;
