#pragma once
#include <glm/glm.hpp>

class Camera {
public:
	Camera();
	~Camera();

public:
	void Reset(const glm::vec3& eye, const glm::vec3& center);

public:
	void Zoom(const float delta);
	void Move(const glm::vec2& delta);
	void Rotate(const glm::vec2& delta);

public:
	const glm::vec3& GetPosition();
	const glm::mat4& GetProjMatrix();
	const glm::mat4& GetViewMatrix();

private:
	glm::vec3 pos_, center_;
	float rspeed_, zspeed_, mspeed_;
	float phi_, theta_;

	glm::mat4 proj_, view_;
};
