#pragma once
#include <gl/glew.h>
#include "texture.h"

class Framebuffer {
public:
	Framebuffer();
	~Framebuffer();

public:
	void Create(GLsizei width, GLsizei height);

	void Bind();
	void Unbind();

	void Clear(GLbitfield buffers);

	int GetWidth() const { return width_; }
	int GetHeight() const { return height_; }

	unsigned GetNativePointer() const { return fbo_; }

	void SetDepthTexture(RenderTexture texture);
#ifdef MRT
	void AddRenderTexture(RenderTexture texture);
	void RemoveRenderTexture(RenderTexture texture);
#else
	void SetRenderTexture(RenderTexture texture);
#endif

	void CreateDepthRenderBuffer();

	int GetRenderTextureCount() const;
	RenderTexture GetDepthTexture();
	RenderTexture GetRenderTexture(int index);

private:
	void PushFramebuffer();
	void PopFramebuffer();

	void PushViewport(int x, int y, int w, int h);
	void PopViewport();

	int UpdateAttachments();
	int FindAttachmentIndex();

private:
	GLuint fbo_;
	GLuint depthRenderbuffer_;

	int attachedRenderTextureCount_;
	int maxRenderTextures_;
	GLenum* attachments_;
	RenderTexture* renderTextures_;
	RenderTexture depthTexture_;

	GLint oldFramebuffer_;
	GLint oldViewport_[4];

	GLsizei width_;
	GLsizei height_;
};
