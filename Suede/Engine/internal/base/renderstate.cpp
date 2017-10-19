#include <cstdarg>
#include "tools/debug.h"
#include "renderstate.h"

CullState::CullState(RenderStateParameter parameter) {
	AssertX(IsValidParamter(parameter, 3,
		Front, Back, Off
	), "invalid paramter for 'Cull'.");
	parameter_ = parameter;
}

void CullState::Bind() {
	oldEnabled_ = glIsEnabled(GL_CULL_FACE);
	glGetIntegerv(GL_CULL_FACE_MODE, &oldMode_);

	Enable(GL_CULL_FACE, parameter_ != Off);
	if (parameter_ != Off) {
		glCullFace(RenderParamterToGLEnum(parameter_));
	}
}

void CullState::Unbind() {
	Enable(GL_CULL_FACE, oldEnabled_);
	glCullFace(oldMode_);
}

DepthTestState::DepthTestState(RenderStateParameter parameter) {
	AssertX(IsValidParamter(parameter, 8,
		Never, Less, LessEqual, Equal, Greater, NotEqual, GreaterEqual, Always
	), "invalid parameter for 'DepthTest'.");

	parameter_ = parameter;
}

void DepthTestState::Bind() {
	oldEnabled_ = glIsEnabled(GL_DEPTH_TEST);
	glGetIntegerv(GL_DEPTH_FUNC, &oldMode_);

	Enable(GL_DEPTH_TEST, parameter_ != Always);
	glDepthFunc(RenderParamterToGLEnum(parameter_));
}

void DepthTestState::Unbind() {
	Enable(GL_DEPTH_TEST, oldEnabled_);
	glDepthFunc(oldMode_);
}

DepthWriteState::DepthWriteState(RenderStateParameter parameter) {
	AssertX(IsValidParamter(parameter, 2,
		On, Off
	), "invalid paramter for 'DepthWrite'.");
	parameter_ = parameter;
}

void DepthWriteState::Bind() {
	glGetIntegerv(GL_DEPTH_WRITEMASK, &oldMask_);
	glDepthMask(parameter_ == On);
}

void DepthWriteState::Unbind() {
	glDepthMask(oldMask_);
}

BlendState::BlendState(RenderStateParameter src, RenderStateParameter dest) {
	AssertX(IsValidParamter(src, 9,
		Off, Zero, One, SrcColor, OneMinusSrcColor, SrcAlpha, OneMinusSrcAlpha, DestAlpha, OneMinusDestAlpha
	), "invalid paramter for 'Blend'.");

	AssertX(IsValidParamter(dest, 9,
		None, Zero, One, SrcColor, OneMinusSrcColor, SrcAlpha, OneMinusSrcAlpha, DestAlpha, OneMinusDestAlpha
	), "invalid paramter for 'Blend'.");

	src_ = src;
	dest_ = dest;
}

void BlendState::Bind() {
	oldEnabled_ = glIsEnabled(GL_BLEND);
	glGetIntegerv(GL_BLEND_SRC, &oldSrc_);
	glGetIntegerv(GL_BLEND_DST, &oldDest_);

	Enable(GL_BLEND, src_ != Off);
	glBlendFunc(RenderParamterToGLEnum(src_), RenderParamterToGLEnum(dest_));
}

void BlendState::Unbind() {
	Enable(GL_BLEND, oldEnabled_);
	glBlendFunc(oldSrc_, oldDest_);
}

void RenderState::Enable(GLenum cap, GLboolean enable) {
	if (enable) { glEnable(cap); }
	else { glDisable(cap); }
}

bool RenderState::IsValidParamter(RenderStateParameter parameter, int count, ...) {
	va_list vl;
	va_start(vl, count);

	int i = 0;
	for (; i < count; ++i) {
		if (parameter == va_arg(vl, RenderStateParameter)) {
			break;
		}
	}

	va_end(vl);

	return (i < count);
}

GLenum RenderState::RenderParamterToGLEnum(RenderStateParameter parameter) {
	GLenum value = GL_NONE;
	switch (parameter) {
		case Front:
			value = GL_FRONT;
			break;
		case Back:
			value = GL_BACK;
			break;
		case Never:
			value = GL_NEVER;
			break;
		case Less:
			value = GL_LESS;
			break;
		case LessEqual:
			value = GL_LEQUAL;
			break;
		case Equal:
			value = GL_EQUAL;
			break;
		case Greater:
			value = GL_GREATER;
			break;
		case NotEqual:
			value = GL_NOTEQUAL;
			break;
		case GreaterEqual:
			value = GL_GEQUAL;
			break;
		case Always:
			value = GL_ALWAYS;
			break;
		case Zero:
			value = GL_ZERO;
			break;
		case One:
			value = GL_ONE;
			break;
		case SrcColor:
			value = GL_SRC_COLOR;
			break;
		case OneMinusSrcColor:
			value = GL_ONE_MINUS_SRC_COLOR;
			break;
		case SrcAlpha:
			value = GL_SRC_ALPHA;
			break;
		case OneMinusSrcAlpha:
			value = GL_ONE_MINUS_SRC_ALPHA;
			break;
		case DestAlpha:
			value = GL_DST_ALPHA;
			break;
		case OneMinusDestAlpha:
			value = GL_ONE_MINUS_DST_ALPHA;
			break;
	}

	AssertX(value != GL_NONE, "invalid render paramter " + std::to_string(parameter));
	return value;
}
