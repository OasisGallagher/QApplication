#pragma once
#include <gl/glew.h>

#include "renderer.h"

class RenderOption {
public:
	virtual ~RenderOption() {}
	virtual void Bind() = 0;
	virtual void Unbind() = 0;

protected:
	void Enable(GLenum cap, bool enable);
	GLenum RenderParamterToGLEnum(RenderParameter parameter);
	bool IsValidParamter(RenderParameter parameter, int count, ...);
};

class Cull : public RenderOption {
public:
	Cull(RenderParameter parameter);

	virtual void Bind();
	virtual void Unbind();

private:
	int oldMode_;
	bool oldEnabled_;
	RenderParameter parameter_;
};

class DepthTest : public RenderOption {
public:
	DepthTest(RenderParameter parameter);

	virtual void Bind();
	virtual void Unbind();

private:
	int oldMode_;
	bool oldEnabled_;
	RenderParameter parameter_;
};

class Blend : public RenderOption {
public:
	Blend(RenderParameter src, RenderParameter dest);

	virtual void Bind();
	virtual void Unbind();

private:
	bool oldEnabled_;
	int oldSrc_, oldDest_;
	RenderParameter src_, dest_;
};
