#pragma once
#include "texture.h"
#include "renderer.h"

class ENGINE_EXPORT Graphics {
public:
	static void Blit(RenderTexture src, RenderTexture dest, Renderer renderer);
};
