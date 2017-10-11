#pragma once
#include "posteffect.h"

class Grayscale : public PostEffect {
public:
	virtual void OnRenderImage(RenderTexture src, RenderTexture dest);
};
