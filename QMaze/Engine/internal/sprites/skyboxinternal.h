#pragma once
#include "camera.h"
#include "shader.h"

#include "skybox.h"
#include "internal/sprites/spriteinternal.h"

class SkyboxInternal : public ISkybox, public SpriteInternal {
public:
	SkyboxInternal(Camera camera, std::string* textures);
	~SkyboxInternal();

private:
	Camera camera_;
	Shader shader_;
};
