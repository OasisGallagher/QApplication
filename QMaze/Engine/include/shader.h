#pragma once
#include <vector>
#include <string>
#include <memory>
#include <gl/glew.h>

#include "object.h"

class ENGINE_EXPORT IShader : virtual public IObject {
public:
	virtual bool Load(const std::string& path) = 0;
	virtual GLuint GetNativePointer() const = 0;
};

typedef smart_ptr<IShader> Shader;
