#pragma once
#include <gl/glew.h>

#include "object.h"

class ENGINE_EXPORT Texture : virtual public Object {
public:
	virtual void Bind(GLenum target) = 0;
	virtual void Unbind() = 0;
	virtual GLuint GetNativePointer() const = 0;
};

class ENGINE_EXPORT Texture2D : virtual public Texture {
public:
	virtual bool Load(const std::string& path) = 0;
};

class ENGINE_EXPORT Texture3D : virtual public Texture {
public:
	virtual bool Load(const std::string* textures) = 0;
};
