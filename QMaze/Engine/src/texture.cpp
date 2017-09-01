#include <glm/glm.hpp>

#include "debug.h"
#include "texture.h"
#include "utilities.h"

#include "Magick++.h"

static const void* ReadRawTexture(const std::string& path, int& width, int& height) {
	Magick::Image image;
	static Magick::Blob blob;

	std::string fpath = "resources/" + path;
	try {
		image.read(fpath.c_str());
		image.write(&blob, "RGBA");
		width = image.columns();
		height = image.rows();
	}
	catch (Magick::Error& err) {
		Debug::LogError("failed to load " + fpath + ": " + err.what());
		return nullptr;
	}

	return blob.data();
}

Texture2D::Texture2D() : texture_(0) {
}

Texture2D::~Texture2D() {
	Destroy();
}

void Texture2D::Bind(GLenum target) {
	AssertX(glIsTexture(texture_), "invalid texture");
	glActiveTexture(target);
	glBindTexture(GL_TEXTURE_2D, texture_);
}

void Texture2D::Unbind() {
}

bool Texture2D::Load(const std::string& path) {
	return LoadTexture(path);
}

bool Texture2D::LoadTexture(const std::string& path) {
	int width, height;
	const void* data = ReadRawTexture(path, width, height);
	if (data == nullptr) {
		return false;
	}

	GLuint textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	if (texture_ != 0) {
		Destroy();
	}

	texture_ = textureID;
	return true;
}

void Texture2D::Destroy() {
	if (texture_ != 0) {
		glDeleteTextures(1, &texture_);
		texture_ = 0;
	}
}

Texture3D::Texture3D() : texture_(0) {
}

Texture3D::~Texture3D() {
	Destroy();
}

bool Texture3D::Load(const std::string* textures) {
	GLuint textureID = CreateCubeTexture(textures);
	if (textureID != 0) {
		Destroy();
		texture_ = textureID;
		return true;
	}

	return false;
}

GLuint Texture3D::CreateCubeTexture(const std::string* textures) {
	GLuint textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

	for (int i = 0; i < 6; ++i) {
		int width, height;
		const void* data = ReadRawTexture(textures[i], width, height);
		
		if (data == nullptr) {
			glDeleteTextures(1, &textureID);
			return 0;
		}

		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA,
			width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	}

	return textureID;
}

void Texture3D::Bind(GLenum target) {
	glActiveTexture(target);
	glBindTexture(GL_TEXTURE_CUBE_MAP, texture_);
}

void Texture3D::Unbind() {
}

void Texture3D::Destroy() {
	if (texture_ != 0) {
		glDeleteTextures(1, &texture_);
		texture_ = 0;
	}
}
