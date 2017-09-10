#pragma once
#include <gl/glew.h>

#include "object.h"

class ENGINE_EXPORT Surface : virtual public Object {
public:
	virtual bool Load(const std::string& path) = 0;
	virtual void Render(GLenum mode = GL_TRIANGLES) = 0;
};
