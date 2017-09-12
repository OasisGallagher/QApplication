#include "object.h"

class ENGINE_EXPORT IRenderer : virtual public IObject {
};

typedef smart_ptr<IRenderer> Renderer;
