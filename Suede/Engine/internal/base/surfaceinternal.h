#pragma once

#include <vector>
#include <gl/glew.h>

#include "mesh.h"
#include "surface.h"
#include "internal/base/objectinternal.h"

class SurfaceInternal : public ISurface, public ObjectInternal {
	DEFINE_FACTORY_METHOD(Surface)

public:
	SurfaceInternal();
	~SurfaceInternal();

public:
	virtual void SetAttribute(const SurfaceAttribute& value);
	virtual void AddMesh(Mesh mesh) { meshes_.push_back(mesh); }
	virtual int GetMeshCount() { return meshes_.size(); }
	virtual Mesh GetMesh(int index) { return meshes_[index]; }

	virtual void Bind();
	virtual void Unbind();

private:
	enum {
		VBOPositions,
		VBOTexCoords,
		VBONormals,
		VBOTangents,
		VBOBones,
		VBOIndexes,
		VBOCount,
	};

	void Clear();
	void UpdateGLBuffers(const SurfaceAttribute& attribute);

private:
	GLuint vao_;
	GLuint vbos_[VBOCount];
	std::vector<Mesh> meshes_;
};
