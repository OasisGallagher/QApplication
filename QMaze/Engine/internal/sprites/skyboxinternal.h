#pragma once
#include <string>
#include <gl/glew.h>

#include "internal/misc/loader.h"
#include "internal/sprites/spriteinternal.h"

class Surface;
class Shader;
class Camera;
struct ModelInfo;
class Texture3D;

class SkyboxInternal : public SpriteInternal {
public:
	SkyboxInternal(Camera* camera, std::string* textures);
	~SkyboxInternal();

public:
	virtual void Render();
	virtual Texture3D* GetTexture();

private:
	Surface* mesh_;
	Camera* camera_;
	Shader* shader_;
	Texture3D* texture_;
};
