#pragma once
#include <string>
#include <gl/glew.h>

#include "object.h"
#include "loader.h"

class Surface;
class Shader;
class Camera;
struct ModelInfo;
class Texture3D;

class ENGINE_EXPORT Skybox : public Object {
public:
	Skybox(Camera* camera, std::string* textures);
	~Skybox();

public:
	void Render();
	Texture3D* GetTexture();

	void Rotate(const glm::mat4& rotation);

private:
	Surface* mesh_;
	Camera* camera_;
	Shader* shader_;
	Texture3D* texture_;

	glm::mat4 rotation_;
};
