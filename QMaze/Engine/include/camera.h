#pragma once
#include <glm/glm.hpp>

class RenderTarget;

class ENGINE_EXPORT Camera {
public:
	Camera();
	~Camera();

public:
	enum ClearType {
		ClearColor,
		ClearSkybox,
	};

	void SetColorType(ClearType type);
	void SetClearColor(const glm::vec3& color);
	//void SetClearSkybox(const Skybox);

public:
	void SetRenderTarget(RenderTarget* target);

public:
	void Render();

public:
	void Zoom(const float delta);
	void Move(const glm::vec2& delta);
	void Rotate(const glm::vec2& delta);
	void LookAt(const glm::vec3& eye, const glm::vec3& center);

public:
	void SetAspect(float value) { aspect_ = value; }
	void SetNearClipPlane(float value) { near_ = value; }
	void SetFarClipPlane(float value) { far_ = value; }
	void SetFieldOfView(float value) { near_ = value; }

	float GetAspect() const { return aspect_; }
	float GetNearClipPlane() const { return near_; }
	float GetFarClipPlane() const { return far_; }
	float GetFieldOfView() const {return fieldOfView_;}

	const glm::vec3& GetPosition();
	const glm::mat4& GetProjMatrix();
	const glm::mat4& GetViewMatrix();

private:
	glm::vec3 pos_, center_;
	float phi_, theta_;
	float aspect_;
	float near_, far_;
	float fieldOfView_;

	glm::mat4 proj_, view_;
};
