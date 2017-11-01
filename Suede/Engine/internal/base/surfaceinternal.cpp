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
	: ObjectInternal(ObjectTypeSurface) {
	vao_.Create(VBOCount);
}

SurfaceInternal::~SurfaceInternal() {
	Destroy();
}

void SurfaceInternal::Destroy() {
	vao_.Destroy();
	meshes_.clear();
}

void SurfaceInternal::SetAttribute(const SurfaceAttribute& value) {
	vao_.Bind();
	UpdateGLBuffers(value);
	vao_.Unbind();
}

void SurfaceInternal::UpdateGLBuffers(const SurfaceAttribute& attribute) {
	if (!attribute.positions.empty()) {
		vao_.SetBuffer(VBOPositions, GL_ARRAY_BUFFER, attribute.positions, GL_STATIC_DRAW);
		vao_.SetVertexDataSource(VBOPositions, VertexAttributeIndexPosition, 3, GL_FLOAT, false, 0, 0);
	}

	if (!attribute.texCoords.empty()) {
		vao_.SetBuffer(VBOTexCoords, GL_ARRAY_BUFFER, attribute.texCoords, GL_STATIC_DRAW);
		vao_.SetVertexDataSource(VBOTexCoords, VertexAttributeIndexTexCoord, 2, GL_FLOAT, false, 0, 0);
	}

	if (!attribute.normals.empty()) {
		vao_.SetBuffer(VBONormals, GL_ARRAY_BUFFER, attribute.normals, GL_STATIC_DRAW);
		vao_.SetVertexDataSource(VBONormals, VertexAttributeIndexNormal, 3, GL_FLOAT, false, 0, 0);
	}

	if (!attribute.tangents.empty()) {
		vao_.SetBuffer(VBOTangents, GL_ARRAY_BUFFER, attribute.tangents, GL_STATIC_DRAW);
		vao_.SetVertexDataSource(VBOTangents, VertexAttributeIndexTangent, 3, GL_FLOAT, false, 0, 0);
	}

	if (!attribute.blendAttrs.empty()) {
		vao_.SetBuffer(VBOBones, GL_ARRAY_BUFFER, attribute.blendAttrs, GL_STATIC_DRAW);

		vao_.SetVertexDataSource(VBOBones, VertexAttributeIndexBoneIndexes, 4, GL_INT, false, sizeof(BlendAttribute), 0);
		vao_.SetVertexDataSource(VBOBones, VertexAttributeIndexBoneWeights, 4, GL_FLOAT, false, sizeof(BlendAttribute), (sizeof(unsigned) * BlendAttribute::Quality));
	}

	if (!attribute.indexes.empty()) {
		vao_.SetBuffer(VBOIndexes, GL_ELEMENT_ARRAY_BUFFER, attribute.indexes, GL_STATIC_DRAW);
	}
}

void SurfaceInternal::Bind() {
	vao_.Bind();
	vao_.BindBuffer(VBOIndexes);
}

void SurfaceInternal::Unbind() {
	vao_.Unbind();
	vao_.UnbindBuffer(VBOIndexes);
}
