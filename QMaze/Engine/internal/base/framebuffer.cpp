#include "tools/debug.h"
#include "framebuffer.h"
#include "tools/string.h"
#include "internal/memory/memory.h"
#include "internal/memory/factory.h"
#include "internal/base/textureinternal.h"

Framebuffer::Framebuffer() : oldFramebuffer_(0), depthRenderbuffer_(0){
}

Framebuffer::~Framebuffer() {
	Memory::ReleaseArray(renderTextures_);
	Memory::ReleaseArray(attachments_);

	if (depthRenderbuffer_ != 0) {
		glDeleteRenderbuffers(1, &depthRenderbuffer_);
		depthRenderbuffer_ = 0;
	}
}

void Framebuffer::Create(GLsizei width, GLsizei height) {
	width_ = width, height_ = height;
	attachedRenderTextureCount_ = 0;

	glGenFramebuffers(1, &fbo_);
	glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS, &maxRenderTextures_);
	renderTextures_ = Memory::CreateArray<RenderTexture>(maxRenderTextures_);
	attachments_ = Memory::CreateArray<GLenum>(maxRenderTextures_);

	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	AssertX(status == GL_FRAMEBUFFER_COMPLETE, String::Format("failed to create framebuffer, 0x04x.", status));
}

void Framebuffer::Bind() {
	PushFramebuffer();
	PushViewport(0, 0, width_, height_);

	int count = UpdateAttachments();
	glDrawBuffers(count, attachments_);
}

void Framebuffer::Unbind() {
	PopFramebuffer();
	PopViewport();
}

void Framebuffer::Clear(GLbitfield buffers) {
	PushFramebuffer();
	glClear(buffers);
	PopFramebuffer();
}

int Framebuffer::UpdateAttachments() {
	if (attachedRenderTextureCount_ == 0) {
		attachments_[0] = GL_NONE;
		return 1;
	}

	int count = 0;
	for (int i = 0; i < maxRenderTextures_; ++i) {
		if (renderTextures_[i]) {
			attachments_[count++] = GL_COLOR_ATTACHMENT0 + i;
		}
	}

	return count;
}

void Framebuffer::CreateDepthRenderBuffer() {
	AssertX(depthRenderbuffer_ == 0, "depth texture or render buffer already exists");
	PushFramebuffer();

	glGenRenderbuffers(1, &depthRenderbuffer_);
	glBindRenderbuffer(GL_RENDERBUFFER, depthRenderbuffer_);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width_, height_);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthRenderbuffer_);

	PopFramebuffer();
}

void Framebuffer::SetDepthTexture(RenderTexture texture) {
	PushFramebuffer();
	depthTexture_ = texture;
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, texture->GetNativePointer(), 0);
	PopFramebuffer();
}

int Framebuffer::GetRenderTextureCount() const {
	return attachedRenderTextureCount_;
}

RenderTexture Framebuffer::GetRenderTexture(int index) {
	AssertX(index < attachedRenderTextureCount_, "index out of range");
	return renderTextures_[index];
}

RenderTexture Framebuffer::GetDepthTexture() {
	return depthTexture_;
}

#ifdef MRT
void Framebuffer::AddRenderTexture(RenderTexture texture) {
	int index = FindAttachmentIndex();
	AssertX(index >= 0, "too many render textures");

	PushFramebuffer();

	renderTextures_[index] = texture;

	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + index, texture->GetNativePointer(), 0);

	++attachedRenderTextureCount_;

	PopFramebuffer();
}

void Framebuffer::RemoveRenderTexture(RenderTexture texture) {
	if (!texture) { return; }

	for (int i = 0; i < maxRenderTextures_; ++i) {
		if (renderTextures_[i] == texture) {
			renderTextures_[i].reset();
			glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, 0, 0);
		}
	}
}

#else

void Framebuffer::SetRenderTexture(RenderTexture texture) {
	PushFramebuffer();

	renderTextures_[0] = texture;

	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, texture ? texture->GetNativePointer() : 0, 0);

	attachedRenderTextureCount_ = texture ? 1 : 0;

	PopFramebuffer();
}
#endif	// MRT

int Framebuffer::FindAttachmentIndex() {
	for (int i = 0; i < maxRenderTextures_; ++i) {
		if (!renderTextures_[i]) {
			return i;
		}
	}

	return -1;
}

void Framebuffer::PushFramebuffer() {
	glGetIntegerv(GL_FRAMEBUFFER_BINDING, &oldFramebuffer_);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo_);
}

void Framebuffer::PopFramebuffer() {
	glBindFramebuffer(GL_FRAMEBUFFER, oldFramebuffer_);
	oldFramebuffer_ = 0;
}

void Framebuffer::PushViewport(int x, int y, int w, int h) {
	glGetIntegerv(GL_VIEWPORT, oldViewport_);
	glViewport(x, y, w, h);
}

void Framebuffer::PopViewport() {
	glViewport(oldViewport_[0], oldViewport_[1], oldViewport_[2], oldViewport_[3]);
	std::fill(oldViewport_, oldViewport_ + 4, 0);
}
