#pragma once
#include <gl/glew.h>
#include <glm/glm.hpp>

#include "texture.h"

class Framebuffer0 {
public:
	Framebuffer0();
	virtual ~Framebuffer0() {}

public:
	virtual void Create(int width, int height);

	virtual void Bind();
	virtual void Unbind();

public:
	int GetWidth() { return width_; }
	int GetHeight() { return height_; }

	void SetClearColor(const glm::vec3& value) { clearColor_ = value; }
	glm::vec3 GetClearColor() { return clearColor_; }

	void Clear(int buffers = GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	unsigned GetNativePointer() { return fbo_; }

protected:
	void PushFramebuffer();
	void PopFramebuffer();

	void PushViewport(int x, int y, int w, int h);
	void PopViewport();

protected:
	GLuint fbo_;

private:
	GLsizei width_;
	GLsizei height_;

	glm::vec3 clearColor_;

	GLint oldFramebuffer_;
	GLint oldViewport_[4];
};

class Framebuffer : public Framebuffer0 {
public:
	Framebuffer();
	~Framebuffer();

public:
	virtual void Create(int width, int height);
	virtual void Bind();

public:
	void SetDepthTexture(RenderTexture texture);

#ifdef MRT
	void AddRenderTexture(RenderTexture texture);
	void RemoveRenderTexture(RenderTexture texture);
#else
	void SetRenderTexture(RenderTexture texture);
#endif

	void CreateDepthRenderBuffer();

	int GetRenderTextureCount();
	RenderTexture GetDepthTexture();
	RenderTexture GetRenderTexture(int index);

private:
	int UpdateAttachments();
	int FindAttachmentIndex();

private:
	GLuint depthRenderbuffer_;

	int attachedRenderTextureCount_;
	int maxRenderTextures_;
	GLenum* attachments_;
	RenderTexture* renderTextures_;
	RenderTexture depthTexture_;
};
