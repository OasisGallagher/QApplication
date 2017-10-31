#pragma once
#include <gl/glew.h>
#include "vertexarray.h"

class TransformFeedback {
public:
	TransformFeedback();
	~TransformFeedback();

public:
	void Create(size_t n, size_t size);
	void Destroy();

	void Bind();
	void Unbind();

private:
	size_t tfCount_;
	VertexArray vao_;

	GLuint* tfs_;
};
