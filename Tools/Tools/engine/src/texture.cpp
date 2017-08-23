#include <glm/glm.hpp>

#include "debug.h"
#include "texture.h"
#include "utilities.h"

#include "Magick++.h"

Texture2D::Texture2D() : texture_(0) {
}

Texture2D::~Texture2D() {
	Destroy();
}

void Texture2D::Bind(GLenum target) {
	Assert(glIsTexture(texture_), "invalid texture");
	glActiveTexture(target);
	glBindTexture(GL_TEXTURE_2D, texture_);
}

void Texture2D::Unbind() {
}

bool Texture2D::Load(const std::string& path) {
	return LoadTexture(path);
}

bool Texture2D::LoadTexture(const std::string& path) {
	Magick::Image image;
	Magick::Blob blob;
	try{
		image.read(path.c_str());
		image.write(&blob, "RGBA");
	}
	catch (Magick::Error& err) {
		Debug::LogError("failed to load " + path + ": " + err.what());
		return false;
	}

	GLuint textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.columns(), image.rows(), 0, GL_RGBA, GL_UNSIGNED_BYTE, blob.data());
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
		Magick::Image image;
		Magick::Blob blob;
		try{
			image.read(textures[i].c_str());
			image.write(&blob, "RGBA");
		}
		catch (Magick::Error& err) {
			Debug::LogError("failed to load texture " + textures[i] + ": " + err.what());
			return 0;
		}

		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA,
			image.columns(), image.rows(), 0, GL_RGBA, GL_UNSIGNED_BYTE, blob.data());

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

RandomTexture::RandomTexture() {
	texture_ = 0;
}

RandomTexture::~RandomTexture() {
}

void RandomTexture::Destroy() {
	if (texture_ != 0) {
		glDeleteTextures(1, &texture_);
		texture_ = 0;
	}
}

bool RandomTexture::Load(unsigned size) {
	GLuint textureID = LoadRandomTexture(size);
	if (textureID != 0) {
		Destroy();
		texture_ = textureID;
		return true;
	}

	return false;
}

void RandomTexture::Bind(GLenum target) {
	Assert(glIsTexture(texture_), "invalid texture.");
	glActiveTexture(target);
	glBindTexture(GL_TEXTURE_1D, texture_);
}

void RandomTexture::Unbind() {
}

GLuint RandomTexture::LoadRandomTexture(unsigned size) {
	glm::vec3* data = new glm::vec3[size];
	for (unsigned i = 0; i < size; ++i) {
		data[i].x = (float)rand() / RAND_MAX;
		data[i].y = (float)rand() / RAND_MAX;
		data[i].z = (float)rand() / RAND_MAX;
	}

	GLuint textureID = 0;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_1D, textureID);

	// It is a 1D texture with the GL_RGB internal format and floating point data type. 
	// This means that every element is a vector of 3 floating point values.
	glTexImage1D(GL_TEXTURE_1D, 0, GL_RGB, size, 0, GL_RGB, GL_FLOAT, data);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// This allows us to use any texture coordinate to access the texture. 
	// If the texture coordinate is more than 1.0 it is simply wrapped around so it always retrieves a valid value.
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_REPEAT);

	delete[] data;

	return textureID;
}
