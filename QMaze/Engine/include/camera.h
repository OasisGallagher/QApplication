#pragma once
#include <glm/glm.hpp>

#include "skybox.h"
#include "sprite.h"

enum ClearType {
	ClearTypeColor,
	ClearTypeSkybox,
};

class ENGINE_EXPORT ICamera : virtual public ISprite {
public:
	virtual void SetClearType(ClearType value) = 0;
	virtual ClearType GetClearType() = 0;

	virtual void SetSkybox(Skybox value) = 0;
	virtual Skybox GetSkybox() = 0;

	virtual void SetClearColor(const glm::vec3& value) = 0;
	virtual void SetRenderTexture(RenderTexture value) = 0;

	virtual void SetAspect(float value) = 0;
	virtual void SetNearClipPlane(float value) = 0;
	virtual void SetFarClipPlane(float value) = 0;
	virtual void SetFieldOfView(float value) = 0;

	virtual float GetAspect() const = 0;
	virtual float GetNearClipPlane() const = 0;
	virtual float GetFarClipPlane() const = 0;
	virtual float GetFieldOfView() const = 0;

	virtual const glm::mat4& GetProjectionMatrix() = 0;
};

typedef smart_ptr<ICamera> Camera;
