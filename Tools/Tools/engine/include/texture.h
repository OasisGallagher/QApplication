#pragma once
#include <string>

#include <gl/glew.h>

class Texture2D {
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

class Texture3D {
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

class RandomTexture {
public:
	RandomTexture();
	~RandomTexture();

public:
	bool Load(unsigned size);

	void Bind(GLenum target);
	void Unbind();

private:
	GLuint LoadRandomTexture(unsigned size);
	void Destroy();

private:
	GLuint texture_;
};
