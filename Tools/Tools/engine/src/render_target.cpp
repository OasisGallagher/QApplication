#include "debug.h"
#include "render_state.h"
#include "render_target.h"

RenderTarget::RenderTarget() {
}

RenderTarget::~RenderTarget() {
	if (index_ > 0) {
		glDeleteBuffers(index_, textures_);
	}

	delete[] textures_;

	if (glIsRenderbuffer(depth_)) {
		glDeleteRenderbuffers(1, &depth_);
		depth_ = 0;
	}

	if (glIsTexture(depth_)) {
		glDeleteTextures(1, &depth_);
	}
}

void RenderTarget::Create(GLsizei width, GLsizei height) {
	width_ = width, height_ = height;
	index_ = 0;

	glGenFramebuffers(1, &fbo_);
	glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS, (GLint*)&max_textures_);
	textures_ = new GLuint[max_textures_];
}

void RenderTarget::Bind(GLenum* buffers, GLuint count) {
	RenderState::PushFramebuffer(fbo_);

	GLenum* colors = nullptr;

	if (buffers == nullptr) {
		if (index_ > 0) {
			colors = new GLenum[index_];
			for (GLuint i = 0; i < index_; ++i) {
				colors[i] = GL_COLOR_ATTACHMENT0 + i;
			}

			count = index_;
		}
		else {
			colors = new GLenum[1];
			colors[0] = GL_NONE;
			count = 1;
		}

		buffers = colors;

	}

	if (buffers != nullptr) {
		glDrawBuffers(count, buffers);
	}

	delete[] colors;

	RenderState::PushViewport(0, 0, width_, height_);
}

void RenderTarget::Unbind() {
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	RenderState::PopViewport();
}

void RenderTarget::Clear(GLbitfield buffers) {
	RenderState::PushFramebuffer(fbo_);
	glClear(buffers);
	RenderState::PopFramebuffer();
}

void RenderTarget::AddDepthRenderBuffer() {
	Assert(depth_ == 0, "depth texture or render buffer already exists");
	RenderState::PushFramebuffer(fbo_);

	glGenRenderbuffers(1, &depth_);
	glBindRenderbuffer(GL_RENDERBUFFER, depth_);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width_, height_);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depth_);

	RenderState::PopFramebuffer();
}

void RenderTarget::AddDepthTexture(GLenum internalFormat, GLenum minFilter, GLenum magFilter, GLenum wrapS, GLenum wrapT) {
	Assert(depth_ == 0, "depth texture or render buffer already exists");
	RenderState::PushFramebuffer(fbo_);

	glGenTextures(1, &depth_);
	glBindTexture(GL_TEXTURE_2D, depth_);

	GLenum type = internalFormat == GL_DEPTH32F_STENCIL8 ? GL_FLOAT_32_UNSIGNED_INT_24_8_REV : GL_FLOAT;
	GLenum format = internalFormat == GL_DEPTH32F_STENCIL8 ? GL_DEPTH_STENCIL : GL_DEPTH_COMPONENT;
	glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width_, height_, 0, format, type, nullptr);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapS);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapT);

	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depth_, 0);

	RenderState::PopFramebuffer();
}

GLuint RenderTarget::GetRenderTexture(GLuint index) const {
	Assert(index < index_, "index out of range");
	return textures_[index];
}

GLuint RenderTarget::GetDepthTexture() const {
	if (!glIsRenderbuffer(depth_)) {
		return depth_;
	}

	return 0;
}

void RenderTarget::AddRenderTexture(GLenum internalFormat, GLenum minFilter, GLenum magFilter, GLenum wrapS, GLenum wrapT) {
	Assert(index_ < max_textures_, "too many render textures");

	GLuint& texture = textures_[index_++];

	RenderState::PushFramebuffer(fbo_);

	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width_, height_, 0, GL_RGB, GL_FLOAT, nullptr);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapS);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapT);

	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + index_ - 1, texture, 0);

	RenderState::PopFramebuffer();
}
