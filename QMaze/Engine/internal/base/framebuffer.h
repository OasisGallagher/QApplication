#pragma once
#include <gl/glew.h>
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
	int GetWidth() const { return width_; }
	int GetHeight() const { return height_; }
	void Clear(int buffers);

	unsigned GetNativePointer() const { return fbo_; }

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

	int GetRenderTextureCount() const;
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
