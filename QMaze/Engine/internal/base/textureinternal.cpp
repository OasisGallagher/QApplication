#include <Magick++.h>
#include <glm/glm.hpp>

#include "tools/debug.h"
#include "textureinternal.h"

void TextureInternal::Bind(GLenum location) {
	AssertX(glIsTexture(texture_), "invalid texture");
	location_ = location;
	glActiveTexture(location);
	glBindTexture(GetGLTextureType(), texture_);
}

void TextureInternal::Unbind() {
	glActiveTexture(location_);
	glBindTexture(GetGLTextureType(), 0);
}

const void* TextureInternal::ReadRawTexture(const std::string& path, int& width, int& height) {
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

void TextureInternal::DestroyTexture() {
	if (texture_ != 0) {
		glDeleteTextures(1, &texture_);
		texture_ = 0;
	}
}

Texture2DInternal::Texture2DInternal() : TextureInternal(ObjectTypeTexture2D) {
}

Texture2DInternal::~Texture2DInternal() {
	DestroyTexture();
}

bool Texture2DInternal::Load(const std::string& path) {
	int width, height;
	const void* data = ReadRawTexture(path, width, height);
	if (data == nullptr) {
		return false;
	}

	DestroyTexture();

	width_ = width;
	height_ = height;

	GLint oldBindingTexture = 0;
	glGetIntegerv(GL_TEXTURE_2D, &oldBindingTexture);

	glGenTextures(1, &texture_);
	glBindTexture(GL_TEXTURE_2D, texture_);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glBindTexture(GL_TEXTURE_2D, oldBindingTexture);

	return true;
}

TextureCubeInternal::TextureCubeInternal() : TextureInternal(ObjectTypeTextureCube) {
}

TextureCubeInternal::~TextureCubeInternal() {
	DestroyTexture();
}

bool TextureCubeInternal::Load(const std::string(&textures)[6]) {
	DestroyTexture();

	GLint oldBindingTexture = 0;
	glGetIntegerv(GL_TEXTURE_BINDING_CUBE_MAP, &oldBindingTexture);

	glGenTextures(1, &texture_);
	glBindTexture(GL_TEXTURE_CUBE_MAP, texture_);

	for (int i = 0; i < 6; ++i) {
		int width, height;
		const void* data = ReadRawTexture(textures[i], width, height);

		if (data == nullptr) {
			DestroyTexture();
			return false;
		}

		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA,
			width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	}

	glBindTexture(GL_TEXTURE_CUBE_MAP, oldBindingTexture);
	return true;
}

RenderTextureInternal::RenderTextureInternal() :TextureInternal(ObjectTypeRenderTexture) {	
}

bool RenderTextureInternal::Load(RenderTextureFormat format, int width, int height) {
	DestroyTexture();
	
	width_ = width;
	height_ = height;

	GLint oldBindingTexture = 0;
	glGetIntegerv(GL_TEXTURE_2D, &oldBindingTexture);

	glGenTextures(1, &texture_);
	glBindTexture(GL_TEXTURE_2D, texture_);

	std::pair<GLenum, GLenum> formats = RenderTextureFormatToGLEnum(format);

	glTexImage2D(GL_TEXTURE_2D, 0, formats.first, width, height, 0, formats.second, GL_FLOAT, nullptr);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);

	glBindTexture(GL_TEXTURE_2D, oldBindingTexture);

	return true;
}

std::pair<GLenum, GLenum> RenderTextureInternal::RenderTextureFormatToGLEnum(RenderTextureFormat renderTextureFormat) {
	GLenum internalFormat = GL_RGBA;
	GLenum format = GL_RGBA;

	switch (renderTextureFormat) {
		case  Rgba:
			internalFormat = GL_RGBA;
			break;
		case RgbaHdr:
			internalFormat = GL_RGBA32F;
			break;
		case Depth:
			internalFormat = GL_DEPTH_COMPONENT24;
			format = GL_DEPTH_COMPONENT;
			break;
		default:
			Debug::LogError("invalid render texture format: " + std::to_string(renderTextureFormat));
			break;
	}

	return std::make_pair(internalFormat, format);
}
