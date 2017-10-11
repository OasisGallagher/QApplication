#pragma once
#include <string>
#include <gl/glew.h>

#include "texture.h"
#include "internal/base/objectinternal.h"

class TextureInternal : virtual public ITexture, public ObjectInternal {
public:
	TextureInternal(ObjectType type) :ObjectInternal(type), texture_(0), width_(0), height_(0) {
	}

public:
	virtual void Bind(GLenum location);
	virtual void Unbind();
	virtual unsigned GetNativePointer() { return texture_; }
	
	virtual int GetWidth() { return width_; }
	virtual int GetHeight() { return height_; }

protected:
	virtual GLenum GetGLTextureType() = 0;
	void DestroyTexture();

protected:
	const void* ReadRawTexture(const std::string& path, int& width, int& height);

protected:
	int width_, height_;
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
	virtual GLenum GetGLTextureType() { return GL_TEXTURE_2D; }
};

class TextureCubeInternal : public ITextureCube, public TextureInternal {
	DEFINE_FACTORY_METHOD(TextureCube)

public:
	TextureCubeInternal();
	~TextureCubeInternal();

public:
	bool Load(const std::string (&textures)[6]);

protected:
	virtual GLenum GetGLTextureType() { return GL_TEXTURE_CUBE_MAP; }
};

class RenderTextureInternal : public IRenderTexture, public TextureInternal {
	DEFINE_FACTORY_METHOD(RenderTexture)

public:
	RenderTextureInternal();

public:
	bool Load(RenderTextureFormat format, int width, int height);

protected:
	virtual GLenum GetGLTextureType() { return GL_TEXTURE_2D; }
	std::pair<GLenum, GLenum> RenderTextureFormatToGLEnum(RenderTextureFormat renderTextureFormat);
};
