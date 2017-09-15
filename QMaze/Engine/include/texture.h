#pragma once
#include <gl/glew.h>

#include "object.h"

class ENGINE_EXPORT ITexture : virtual public IObject {
public:
	virtual void Bind(GLenum location) = 0;
	virtual void Unbind() = 0;
	virtual unsigned GetNativePointer() const = 0;

	virtual int GetWidth() const = 0;
	virtual int GetHeight() const = 0;
};

class ENGINE_EXPORT ITexture2D : virtual public ITexture {
public:
	virtual bool Load(const std::string& path) = 0;
};

class ENGINE_EXPORT ITextureCube : virtual public ITexture {
public:
	virtual bool Load(const std::string(&textures)[6]) = 0;
};

enum RenderTextureFormat {
	RenderTextureFormatRgba,
	RenderTextureFormatRgbaHdr,
	RenderTextureFormatDepth,
};

class ENGINE_EXPORT IRenderTexture : virtual public ITexture {
public:
	virtual bool Load(RenderTextureFormat format, int width, int height) = 0;
};

typedef smart_ptr<ITexture> Texture;
typedef smart_ptr<ITexture2D> Texture2D;
typedef smart_ptr<ITextureCube> TextureCube;
typedef smart_ptr<IRenderTexture> RenderTexture;
