#pragma once
#include "object.h"
#include "texture.h"

class ENGINE_EXPORT ISkybox : virtual public IObject {
public:
};

typedef smart_ptr<ISkybox> Skybox;
