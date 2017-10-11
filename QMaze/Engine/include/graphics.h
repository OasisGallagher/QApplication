#pragma once
#include "texture.h"
#include "renderer.h"

class ENGINE_EXPORT Graphics {
public:
	void Blit(RenderTexture src, RenderTexture dest, Renderer renderer);

private:
	friend class Engine;
	Graphics() {}
};
