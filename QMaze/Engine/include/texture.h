#pragma once
#include <gl/glew.h>

#include "object.h"

class ENGINE_EXPORT ITexture : virtual public IObject {
public:
	virtual void Bind(GLenum location) = 0;
	virtual void Unbind() = 0;
	virtual GLuint GetNativePointer() const = 0;
};

class ENGINE_EXPORT ITexture2D : virtual public ITexture {
public:
	virtual bool Load(const std::string& path) = 0;
};

class ENGINE_EXPORT ITexture3D : virtual public ITexture {
public:
	virtual bool Load(const std::string* textures) = 0;
};

typedef smart_ptr<ITexture> Texture;
typedef smart_ptr<ITexture2D> Texture2D;
typedef smart_ptr<ITexture3D> Texture3D;
