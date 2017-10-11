#include "graphics.h"

class GraphicsInternal : public IGraphics {
public:
	virtual void Blit(RenderTexture src, RenderTexture dest, Renderer renderer);
};
