#pragma once
#include <glm/glm.hpp>

#include "object.h"

class Surface;

class ENGINE_EXPORT Sprite : virtual public Object {
public:
	virtual void SetParent(Sprite* value) = 0;
	virtual Sprite* GetParent() const = 0;

	virtual int GetChildCount() const = 0;
	virtual Sprite* GetChildAt(int i) = 0;

	virtual void SetScale(const glm::vec3& value) = 0;
	virtual void SetPosition(const glm::vec3& value) = 0;
	virtual void SetEulerAngles(const glm::vec3& value) = 0;

	virtual glm::vec3 GetScale() const = 0;
	virtual glm::vec3 GetPosition() const = 0;
	virtual glm::vec3 GetEulerAngles() const = 0;

	virtual glm::mat4 GetLocalToWorldMatrix() = 0;
	virtual glm::mat4 GetWorldToLocalMatrix() = 0;

	virtual glm::vec3 GetLocalToWorldPosition(const glm::vec3& position) = 0;
	virtual glm::vec3 GetWorldToLocalPosition(const glm::vec3& position) = 0;

	virtual void Update() = 0;

	virtual void SetSurface(Surface* value) = 0;
	virtual Surface* GetSurface() = 0;
};