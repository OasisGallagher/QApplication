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

class Cull : public RenderState {
public:
	Cull(RenderStateParameter parameter);

	virtual void Bind();
	virtual void Unbind();

private:
	int oldMode_;
	GLboolean oldEnabled_;
	RenderStateParameter parameter_;
};

class DepthTest : public RenderState {
public:
	DepthTest(RenderStateParameter parameter);

	virtual void Bind();
	virtual void Unbind();

private:
	int oldMode_;
	GLboolean oldEnabled_;
	RenderStateParameter parameter_;
};

class DepthWrite : public RenderState {
public:
	DepthWrite(RenderStateParameter parameter);

	virtual void Bind();
	virtual void Unbind();

private:
	GLint oldMask_;
	RenderStateParameter parameter_;
};

class Blend : public RenderState {
public:
	Blend(RenderStateParameter src, RenderStateParameter dest);

	virtual void Bind();
	virtual void Unbind();

private:
	GLboolean oldEnabled_;
	int oldSrc_, oldDest_;
	RenderStateParameter src_, dest_;
};
