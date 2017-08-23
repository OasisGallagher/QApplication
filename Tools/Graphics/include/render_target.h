#pragma once
#include <gl/glew.h>

#include "defs.h"

class GRAPHICS_EXPORT RenderTarget {
public:
	RenderTarget();
	~RenderTarget();

public:
	void Create(GLsizei width, GLsizei height);

	void Bind(GLenum* buffers, GLuint count);
	void Unbind();

	void Clear(GLbitfield buffers);

	void AddRenderTexture(GLenum internalFormat,
						  GLenum minFilter,
						  GLenum magFilter,
						  GLenum wrapS,
						  GLenum wrapT);

	void AddDepthTexture(GLenum internalFormat,
						 GLenum minFilter,
						 GLenum magFilter,
						 GLenum wrapS,
						 GLenum wrapT);

	void AddDepthRenderBuffer();

	GLuint GetDepthTexture() const;
	GLuint GetRenderTexture(GLuint index) const;

private:
	GLuint fbo_;
	GLuint depth_;

	GLuint index_;
	GLuint* textures_;
	GLuint max_textures_;

	GLsizei width_;
	GLsizei height_;
};
