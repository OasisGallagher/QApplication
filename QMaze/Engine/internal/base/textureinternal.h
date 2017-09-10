#pragma once
#include <string>
#include <gl/glew.h>

#include "texture.h"
#include "internal/base/objectinternal.h"

class TextureInternal : virtual public Texture, public ObjectInternal {
public:
	TextureInternal(ObjectType type) :ObjectInternal(type) {

	}

public:
	virtual void Bind(GLenum target);
	virtual void Unbind();
	virtual GLuint GetNativePointer() const { return texture_; }

protected:
	virtual GLenum GetBindTarget() = 0;

protected:
	const void* ReadRawTexture(const std::string& path, int& width, int& height);

protected:
	GLuint texture_;
};

class Texture2DInternal : public Texture2D, public TextureInternal {
public:
	Texture2DInternal();
	~Texture2DInternal();

public:
	virtual bool Load(const std::string& path);

protected:
	virtual GLenum GetBindTarget() { return GL_TEXTURE_2D; }

private:
	bool LoadTexture(const std::string& path);
	void Destroy();
};

class Texture3DInternal : public Texture3D, public TextureInternal {
public:
	Texture3DInternal();
	~Texture3DInternal();

public:
	bool Load(const std::string* textures);

protected:
	virtual GLenum GetBindTarget() { return GL_TEXTURE_3D; }

private:
	void Destroy();
	GLuint CreateCubeTexture(const std::string* textures);
};
