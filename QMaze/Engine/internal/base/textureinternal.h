#pragma once
#include <string>
#include <gl/glew.h>

#include "texture.h"
#include "internal/base/objectinternal.h"

class TextureInternal : virtual public ITexture, public ObjectInternal {
public:
	TextureInternal(ObjectType type) :ObjectInternal(type), texture_(0) {

	}

public:
	virtual void Bind(GLenum location);
	virtual void Unbind();
	virtual GLuint GetNativePointer() const { return texture_; }

protected:
	virtual GLenum GetTextureType() = 0;

protected:
	const void* ReadRawTexture(const std::string& path, int& width, int& height);

protected:
	GLuint texture_;
	GLenum location_;
};

class Texture2DInternal : public ITexture2D, public TextureInternal {
	DEFINE_FACTORY_METHOD(Texture2D)

public:
	Texture2DInternal();
	~Texture2DInternal();

public:
	virtual bool Load(const std::string& path);

protected:
	virtual GLenum GetTextureType() { return GL_TEXTURE_2D; }

private:
	bool LoadTexture(const std::string& path);
	void Destroy();
};

class Texture3DInternal : public ITexture3D, public TextureInternal {
	DEFINE_FACTORY_METHOD(Texture3D)

public:
	Texture3DInternal();
	~Texture3DInternal();

public:
	bool Load(const std::string* textures);

protected:
	virtual GLenum GetTextureType() { return GL_TEXTURE_3D; }

private:
	void Destroy();
	GLuint CreateCubeTexture(const std::string* textures);
};
