#pragma once
#include <glm/glm.hpp>

#include "camera.h"
#include "skybox.h"
#include "internal/base/objectinternal.h"
#include "internal/sprites/spriteinternal.h"

class CameraInternal : public ICamera, public SpriteInternal {
	DEFINE_FACTORY_METHOD(Camera)

public:
	CameraInternal();
	~CameraInternal();

public:
	virtual void SetClearType(ClearType type);
	virtual ClearType GetClearType();

	virtual void SetSkybox(Skybox skybox);
	virtual Skybox GetSkybox();

	virtual void SetClearColor(const glm::vec3& color);

public:
	virtual void Update();

public:
	virtual void SetAspect(float value) { aspect_ = value; }
	virtual void SetNearClipPlane(float value) { near_ = value; }
	virtual void SetFarClipPlane(float value) { far_ = value; }
	virtual void SetFieldOfView(float value) { near_ = value; }

	virtual float GetAspect() const { return aspect_; }
	virtual float GetNearClipPlane() const { return near_; }
	virtual float GetFarClipPlane() const { return far_; }
	virtual float GetFieldOfView() const {return fieldOfView_;}

	virtual const glm::mat4& GetProjectionMatrix();

private:
	void ClearFramebuffer();
	void RenderSprite(Sprite sprite);

private:
	float aspect_;
	float near_, far_;
	float fieldOfView_;

	Skybox skybox_;
	ClearType clearType_;
	glm::mat4 projection_;
};
