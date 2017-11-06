#pragma once
#include <gl/glew.h>

#include "renderer.h"

class RenderState {
public:
	virtual ~RenderState() {}
	virtual void Initialize(RenderStateParameter parameter0, RenderStateParameter parameter1) = 0;
	virtual void Bind() = 0;
	virtual void Unbind() = 0;

protected:
	void Enable(GLenum cap, GLboolean enable);
	GLenum RenderParamterToGLEnum(RenderStateParameter parameter);
	bool IsValidParamter(RenderStateParameter parameter, int count, ...);
};

class CullState : public RenderState {
public:
	virtual void Initialize(RenderStateParameter parameter0, RenderStateParameter);
	virtual void Bind();
	virtual void Unbind();

private:
	int oldMode_;
	GLboolean oldEnabled_;
	RenderStateParameter parameter_;
};

class DepthTestState : public RenderState {
public:
	virtual void Initialize(RenderStateParameter parameter0, RenderStateParameter);
	virtual void Bind();
	virtual void Unbind();

private:
	GLenum oldMode_;
	GLboolean oldEnabled_;
	RenderStateParameter parameter_;
};

class DepthWriteState : public RenderState {
public:
	virtual void Initialize(RenderStateParameter parameter0, RenderStateParameter);
	virtual void Bind();
	virtual void Unbind();

private:
	GLint oldMask_;
	RenderStateParameter parameter_;
};

class RasterizerDiscardState : public RenderState {
public:
	virtual void Initialize(RenderStateParameter parameter0, RenderStateParameter);

	virtual void Bind();
	virtual void Unbind();

private:
	GLboolean oldEnabled_;
	RenderStateParameter parameter_;
};

class BlendState : public RenderState {
public:
	virtual void Initialize(RenderStateParameter parameter0, RenderStateParameter parameter1);

	virtual void Bind();
	virtual void Unbind();

private:
	GLboolean oldEnabled_;
	int oldSrc_, oldDest_;
	RenderStateParameter src_, dest_;
};
