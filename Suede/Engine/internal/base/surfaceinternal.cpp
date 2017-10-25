#include "tools/mathf.h"
#include "surfaceinternal.h"
#include "internal/base/materialinternal.h"

MeshInternal::MeshInternal() :ObjectInternal(ObjectTypeMesh), vertexCount_(0), baseVertex_(0), baseIndex_(0) {
}

void MeshInternal::SetTriangles(unsigned vertexCount, unsigned baseVertex, unsigned baseIndex) {
	vertexCount_ = vertexCount;
	baseVertex_ = baseVertex;
	baseIndex_ = baseIndex;
}

void MeshInternal::GetTriangles(unsigned& vertexCount, unsigned& baseVertex, unsigned& baseIndex) {
	vertexCount = vertexCount_;
	baseVertex = baseVertex_;
	baseIndex = baseIndex_;
}

SurfaceInternal::SurfaceInternal() 
	: ObjectInternal(ObjectTypeSurface), vao_(0) {
	std::fill(vbos_, vbos_ + CountOf(vbos_), 0);

	glGenVertexArrays(1, &vao_);

	Bind();
	glGenBuffers(CountOf(vbos_), vbos_);
	Unbind();
}

SurfaceInternal::~SurfaceInternal() {
	Clear();
}

void SurfaceInternal::Clear() {
	glDeleteVertexArrays(1, &vao_);
	vao_ = 0;

	glDeleteVertexArrays(CountOf(vbos_), vbos_);
	std::fill(vbos_, vbos_ + CountOf(vbos_), 0);

	meshes_.clear();
}

void SurfaceInternal::SetAttribute(const SurfaceAttribute& value) {
	Bind();
	UpdateGLBuffers(value);
	Unbind();
}
void SurfaceInternal::UpdateGLBuffers(const SurfaceAttribute& attribute) {
	if (!attribute.positions.empty()) {
		glBindBuffer(GL_ARRAY_BUFFER, vbos_[VBOPositions]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * attribute.positions.size(), &attribute.positions[0], GL_STATIC_DRAW);
		glEnableVertexAttribArray(VertexAttributeIndexPosition);
		glVertexAttribPointer(VertexAttributeIndexPosition, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
	}

	if (!attribute.texCoords.empty()) {
		glBindBuffer(GL_ARRAY_BUFFER, vbos_[VBOTexCoords]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * attribute.texCoords.size(), &attribute.texCoords[0], GL_STATIC_DRAW);
		glEnableVertexAttribArray(VertexAttributeIndexTexCoord);
		glVertexAttribPointer(VertexAttributeIndexTexCoord, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
	}

	if (!attribute.normals.empty()) {
		glBindBuffer(GL_ARRAY_BUFFER, vbos_[VBONormals]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * attribute.normals.size(), &attribute.normals[0], GL_STATIC_DRAW);
		glEnableVertexAttribArray(VertexAttributeIndexNormal);
		glVertexAttribPointer(VertexAttributeIndexNormal, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
	}

	if (!attribute.tangents.empty()) {
		glBindBuffer(GL_ARRAY_BUFFER, vbos_[VBOTangents]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * attribute.tangents.size(), &attribute.tangents[0], GL_STATIC_DRAW);
		glEnableVertexAttribArray(VertexAttributeIndexTangent);
		glVertexAttribPointer(VertexAttributeIndexTangent, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
	}

	if (!attribute.blendAttrs.empty()) {
		glBindBuffer(GL_ARRAY_BUFFER, vbos_[VBOBones]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(BlendAttribute)* attribute.blendAttrs.size(), &attribute.blendAttrs[0], GL_STATIC_DRAW);

		glEnableVertexAttribArray(VertexAttributeIndexBoneIndexes);
		glVertexAttribIPointer(VertexAttributeIndexBoneIndexes, 4, GL_INT, sizeof(BlendAttribute), nullptr);

		glEnableVertexAttribArray(VertexAttributeIndexBoneWeights);
		glVertexAttribPointer(VertexAttributeIndexBoneWeights, 4, GL_FLOAT, GL_FALSE, sizeof(BlendAttribute), (const GLvoid*)(sizeof(unsigned) * BlendAttribute::Quality));
	}

	if (!attribute.indexes.empty()) {
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbos_[VBOIndexes]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned) * attribute.indexes.size(), &attribute.indexes[0], GL_STATIC_DRAW);
	}
}

void SurfaceInternal::Bind() {
	glBindVertexArray(vao_);
}

void SurfaceInternal::Unbind() {
	glBindVertexArray(0);
}
