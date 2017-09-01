#pragma once
#include <gl/glew.h>

#define PushCapability(Cap, Enabled, Variable)	glGetBooleanv(Cap, &get().Variable); enableCap(Cap, !!Enabled);
#define PopCapability(Cap, Variable)	if (get().Variable) { glEnable(Cap); } else { glDisable(Cap); }

class RenderState {
public:
	static void PushFramebuffer(GLuint fbo) {
		glGetIntegerv(GL_FRAMEBUFFER_BINDING, (GLint*)&get().fbo_);
		glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	}

	static void PopFramebuffer() {
		glBindFramebuffer(GL_FRAMEBUFFER, get().fbo_);
	}

	static void PushViewport(GLint x, GLint y, GLsizei w, GLsizei h) {
		glGetIntegerv(GL_VIEWPORT, (GLint*)get().viewport_);
		glViewport(x, y, w, h);
	}

	static void PopViewport() {
		glViewport(get().viewport_[0], get().viewport_[1], get().viewport_[2], get().viewport_[3]);
	}

	static void PushCullFaceEnabled(GLboolean enabled) {
		PushCapability(GL_CULL_FACE, enabled, cull_face_enabled_);
	}

	static void PopCullFaceEnabled() {
		PopCapability(GL_CULL_FACE, cull_face_enabled_);
	}

	static void PushCullFaceFunc(GLenum mode) {
		glGetIntegerv(GL_CULL_FACE_MODE, (GLint*)&get().cull_face_);
		glCullFace(mode);
	}

	static void PopCullFaceFunc() {
		glCullFace(get().cull_face_);
	}

	static void PushDepthTestEnabled(GLboolean enabled) {
		PushCapability(GL_DEPTH_TEST, enabled, depth_test_enabled_);
	}

	static void PopDepthTestEnabled() {
		PopCapability(GL_DEPTH_TEST, depth_test_enabled_);
	}

	static void PushDepthTestFunc(GLenum mode) {
		glGetIntegerv(GL_DEPTH_FUNC, (GLint*)&get().depth_func_);
		glDepthFunc(mode);
	}

	static void PopDepthTestFunc() {
		glDepthFunc(get().depth_func_);
	}

	static void PushBlendEnabled(GLboolean enabled) {
		PushCapability(GL_BLEND, enabled, blend_enabled_);
	}

	static void PopBlendEnabled() {
		PopCapability(GL_BLEND, blend_enabled_);
	}

	static void PushBlendAlphaFunc(GLenum sfactor, GLenum dfactor) {
		glGetIntegerv(GL_BLEND_SRC_ALPHA, (GLint*)&get().blend_sfactor_);
		glGetIntegerv(GL_BLEND_DST_ALPHA, (GLint*)&get().blend_dfactor_);

		glBlendFunc(sfactor, dfactor);
	}

	static void PopBlendAlphaFunc() {
		glBlendFunc(get().blend_sfactor_, get().blend_dfactor_);
	}

	static void PushProgram(GLuint program) {
		glGetIntegerv(GL_CURRENT_PROGRAM, (GLint*)&get().program_);
		glUseProgram(program);
	}

	static void PopProgram() {
		glUseProgram(get().program_);
	}

private:
	static void enableCap(GLenum cap, bool enable) {
		if (enable) {
			glEnable(cap);
		}
		else {
			glDisable(cap);
		}
	}

private:
	RenderState() {
	}

private:
	static RenderState& get() {
		static RenderState state;
		return state;
	}

private:
	GLenum cull_face_;
	GLenum depth_func_;
	GLenum blend_sfactor_;
	GLenum blend_dfactor_;

	GLuint fbo_;
	GLuint program_;
	GLuint viewport_[4];

	GLboolean blend_enabled_;
	GLboolean cull_face_enabled_;
	GLboolean depth_test_enabled_;
};

#undef PushCapability
#undef PopCapability
