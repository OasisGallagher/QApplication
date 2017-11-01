#pragma once
#include <gl/glew.h>
#include "vertexarrayobject.h"

class TransformFeedback {
public:
	TransformFeedback();
	~TransformFeedback();

public:
	void Create(size_t n, size_t size);
	void Destroy();

	void Bind(int tfbIndex, int vboIndex);
	void Unbind();

private:
	size_t tfCount_;
	VertexArrayObject vao_;

	GLuint* tfbs_;
	GLuint oldTfb_;
	int bindingVboIndex_;
};
