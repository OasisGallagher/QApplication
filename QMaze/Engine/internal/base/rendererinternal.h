#include "renderer.h"
#include "internal/base/objectinternal.h"

class RendererInternal : public IRenderer, public ObjectInternal {
	DEFINE_FACTORY_METHOD(Renderer)

public:
	RendererInternal();

private:

};
