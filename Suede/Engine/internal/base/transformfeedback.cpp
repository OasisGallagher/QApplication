#include "tools/debug.h"
#include "transformfeedback.h"
#include "internal/memory/memory.h"

TransformFeedback::TransformFeedback() : tfs_(nullptr), tfCount_(0) {
}

TransformFeedback::~TransformFeedback() {
	Destroy();
}

void TransformFeedback::Create(size_t n, size_t size) {
	Assert(n > 0 && size > 0);
	Destroy();

	tfCount_ = n;

	vao_.Create(n);
	vao_.Bind();

	tfs_ = Memory::CreateArray<GLuint>(n);
	glCreateTransformFeedbacks(n, tfs_);

	for (size_t i = 0; i < n; ++i) {
		glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, tfs_[i]);
		glBindBuffer(GL_ARRAY_BUFFER, vao_.GetBuffer(i));
		glBufferData(GL_ARRAY_BUFFER, size, nullptr, GL_DYNAMIC_DRAW);

		// This makes this buffer a transform feedback buffer and places it as index zero.
		// We can have the primitives redirected into more than one buffer by binding several buffers at different indices. 
		glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, vao_.GetBuffer(i));

		// Now we have two transform feedback objects with corresponding buffer objects that
		// can serve both as vertex buffers as well as transform feedback buffers.
	}
}

void TransformFeedback::Destroy() {
	if (tfCount_ == 0) {
		return;
	}

	glDeleteTransformFeedbacks(tfCount_, tfs_);
	Memory::ReleaseArray(tfs_);
	tfs_ = nullptr;

	vao_.Destroy();

	tfCount_ = 0;
}
