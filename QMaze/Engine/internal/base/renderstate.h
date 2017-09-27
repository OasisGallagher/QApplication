#pragma once
#include <gl/glew.h>

#include "renderer.h"

class RenderState {
public:
	virtual ~RenderState() {}
	virtual void Bind() = 0;
	virtual void Unbind() = 0;

protected:
	void Enable(GLenum cap, GLboolean enable);
	GLenum RenderParamterToGLEnum(RenderStateParameter parameter);
	bool IsValidParamter(RenderStateParameter parameter, int count, ...);
};

class CullState : public RenderState {
public:
	CullState(RenderStateParameter parameter);

	virtual void Bind();
	virtual void Unbind();

private:
	int oldMode_;
	GLboolean oldEnabled_;
	RenderStateParameter parameter_;
};

class DepthTestState : public RenderState {
public:
	DepthTestState(RenderStateParameter parameter);

	virtual void Bind();
	virtual void Unbind();

private:
	int oldMode_;
	GLboolean oldEnabled_;
	RenderStateParameter parameter_;
};

class DepthWriteState : public RenderState {
public:
	DepthWriteState(RenderStateParameter parameter);

	virtual void Bind();
	virtual void Unbind();

private:
	GLint oldMask_;
	RenderStateParameter parameter_;
};

class BlendState : public RenderState {
public:
	BlendState(RenderStateParameter src, RenderStateParameter dest);

	virtual void Bind();
	virtual void Unbind();

private:
	GLboolean oldEnabled_;
	int oldSrc_, oldDest_;
	RenderStateParameter src_, dest_;
};
