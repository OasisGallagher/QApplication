#pragma once
#include <string>
#include <gl/glew.h>

#include "defs.h"
#include "object.h"

class TexturePrivate : public ObjectPrivate {
public:
	TexturePrivate(ObjectType type) : ObjectPrivate(type) {

	}

public:
	void Bind(GLenum target);
	void Unbind();

public:
	GLuint GetHandle() const { return texture_; }

protected:
	virtual GLenum GetBindTarget() = 0;

protected:
	const void* ReadRawTexture(const std::string& path, int& width, int& height);

protected:
	GLuint texture_;
};

class Texture2DPrivate : public TexturePrivate {
public:
	Texture2DPrivate();
	~Texture2DPrivate();

public:
	bool Load(const std::string& path);

protected:
	virtual GLenum GetBindTarget() { return GL_TEXTURE_2D; }

private:
	bool LoadTexture(const std::string& path);
	void Destroy();
};

class Texture3DPrivate : public TexturePrivate {
public:
	Texture3DPrivate();
	~Texture3DPrivate();

public:
	bool Load(const std::string* textures);

protected:
	virtual GLenum GetBindTarget() { return GL_TEXTURE_3D; }

private:
	void Destroy();
	GLuint CreateCubeTexture(const std::string* textures);
};

class ENGINE_EXPORT Texture : public Object {
	IMPLEMENT_SMART_POINTER(Texture)
};

class ENGINE_EXPORT Texture2D : public Texture {
	IMPLEMENT_SMART_OBJECT(Texture2D)
};

class ENGINE_EXPORT Texture3D : public Texture {
	IMPLEMENT_SMART_OBJECT(Texture3D)
};
