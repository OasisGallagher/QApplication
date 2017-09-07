#include <glm/glm.hpp>

#include "debug.h"
#include "texture.h"
#include "utilities.h"

#include "Magick++.h"

void TexturePrivate::Bind(GLenum target) {
	AssertX(glIsTexture(texture_), "invalid texture");
	glActiveTexture(target);
	glBindTexture(GetBindTarget(), texture_);
}

void TexturePrivate::Unbind() {
	Texture tex = Texture2D::Create();
	glBindTexture(GetBindTarget(), 0);
}

const void* TexturePrivate::ReadRawTexture(const std::string& path, int& width, int& height) {
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

Texture2DPrivate::Texture2DPrivate() : TexturePrivate(ObjectTexture2D) {
}

Texture2DPrivate::~Texture2DPrivate() {
	Destroy();
}

bool Texture2DPrivate::Load(const std::string& path) {
	return LoadTexture(path);
}

bool Texture2DPrivate::LoadTexture(const std::string& path) {
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

void Texture2DPrivate::Destroy() {
	if (texture_ != 0) {
		glDeleteTextures(1, &texture_);
		texture_ = 0;
	}
}

Texture3DPrivate::Texture3DPrivate() : TexturePrivate(ObjectTexture2D) {
}

Texture3DPrivate::~Texture3DPrivate() {
	Destroy();
}

bool Texture3DPrivate::Load(const std::string* textures) {
	GLuint textureID = CreateCubeTexture(textures);
	if (textureID != 0) {
		Destroy();
		texture_ = textureID;
		return true;
	}

	return false;
}

GLuint Texture3DPrivate::CreateCubeTexture(const std::string* textures) {
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

void Texture3DPrivate::Destroy() {
	if (texture_ != 0) {
		glDeleteTextures(1, &texture_);
		texture_ = 0;
	}
}
