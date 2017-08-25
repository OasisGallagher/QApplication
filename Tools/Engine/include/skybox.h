#pragma once
#include <string>
#include <gl/glew.h>

#include "loader.h"

class Mesh;
class Shader;
class Camera;
struct ModelInfo;
class Texture3D;

class ENGINE_EXPORT SkyBox {
public:
	SkyBox(Camera* camera, std::string* textures);
	~SkyBox();

public:
	void Render();
	Texture3D* GetTexture();

	void Rotate(const glm::mat4& rotation);

private:
	Mesh* mesh_;
	Camera* camera_;
	Shader* shader_;
	Texture3D* texture_;

	glm::mat4 rotation_;
};
