#include "vertexarray.h"
#include "tools/debug.h"
#include "internal/memory/memory.h"

VertexArray::VertexArray() : vao_(0), oldVao_(0), vbos_(nullptr), bufferCount_(0) {
}

VertexArray::~VertexArray() {
	Destroy();
}

GLuint VertexArray::GetBuffer(int index) {
	Assert(index >= 0 && index < bufferCount_);
	return vbos_[index];
}

void VertexArray::Bind() {
	AssertX(vao_ != 0, "invalid vao");
	glGetIntegerv(GL_VERTEX_ARRAY_BINDING, (GLint*)&oldVao_);
	glBindVertexArray(vao_);
}

void VertexArray::Unbind() {
	glBindVertexArray(oldVao_);
	oldVao_ = 0;
}

void VertexArray::Create(size_t n) {
	Destroy();

	glGenVertexArrays(1, &vao_);
	Bind();
	
	vbos_ = Memory::Create<GLuint>(n);
	glGenBuffers(n, vbos_);

	Unbind();
}

void VertexArray::Destroy() {
	if (bufferCount_ == 0) {
		return;
	}

	glDeleteVertexArrays(1, &vao_);
	vao_ = 0;

	glDeleteBuffers(bufferCount_, vbos_);
	vbos_ = nullptr;

	bufferCount_ = 0;
}
