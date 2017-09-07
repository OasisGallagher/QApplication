#pragma once
#include <string>
#include <gl/glew.h>

#include "defs.h"

class ENGINE_EXPORT Texture {
public:
	virtual void Bind(GLenum target);
	virtual void Unbind();

protected:
	GLuint texture_;
};

class ENGINE_EXPORT Texture2D : public Texture {
public:
	Texture2D();
	~Texture2D();

public:
	void Bind(GLenum target);
	void Unbind();
	bool Load(const std::string& path);

private:
	bool LoadTexture(const std::string& path);
	void Destroy();

private:
	GLuint texture_;
};

class ENGINE_EXPORT Texture3D {
public:
	Texture3D();
	~Texture3D();

public:
	void Bind(GLenum target);
	void Unbind();

	bool Load(const std::string* textures);

private:
	void Destroy();
	GLuint CreateCubeTexture(const std::string* textures);

private:
	GLuint texture_;
};
