#pragma once
#include <gl/glew.h>

class VertexArray {
public:
	VertexArray();
	~VertexArray();

public:
	void Create(size_t n);
	void Destroy();

	void Bind();
	void Unbind();

	GLuint GetBuffer(int index);

private:
	GLuint vao_;
	GLuint oldVao_;

	GLuint* vbos_;
	int bufferCount_;
};
