#pragma once
#include <gl/glew.h>
#include "defines.h"
#include "object.h"

class ENGINE_EXPORT IRenderTarget : virtual public IObject {
public:
	virtual void Create(GLsizei width, GLsizei height) = 0;

	virtual void Bind(GLenum* buffers, GLuint count) = 0;
	virtual void Unbind() = 0;

	virtual void Clear(GLbitfield buffers) = 0;

	virtual void AddRenderTexture(GLenum internalFormat,
		GLenum minFilter,
		GLenum magFilter,
		GLenum wrapS,
		GLenum wrapT) = 0;

	virtual void AddDepthTexture(GLenum internalFormat,
		GLenum minFilter,
		GLenum magFilter,
		GLenum wrapS,
		GLenum wrapT) = 0;

	virtual void AddDepthRenderBuffer() = 0;

	virtual GLuint GetDepthTexture() const = 0;
	virtual GLuint GetRenderTexture(GLuint index) const = 0;
};

typedef smart_ptr<IRenderTarget> RenderTarget;
