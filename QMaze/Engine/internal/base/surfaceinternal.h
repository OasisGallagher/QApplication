#pragma once

#include <vector>
#include <gl/glew.h>
#include <glm/glm.hpp>

#include "mesh.h"
#include "surface.h"
#include "material.h"
#include "internal/base/objectinternal.h"

struct aiNode;
struct aiMesh;
struct aiScene;
struct aiNodeAnim;
struct aiAnimation;

class SurfaceInternal : public ISurface, public ObjectInternal {
	DEFINE_FACTORY_METHOD(Surface)

public:
	SurfaceInternal();
	~SurfaceInternal();

public:
	virtual bool Load(const std::string& path);

	virtual void SetAttribute(const SurfaceAttribute& value);
	virtual void AddMesh(Mesh mesh);

	virtual void Bind();
	virtual void Unbind();

private:
	enum {
		VBOPositions,
		VBOUVs,
		VBONormals,
		VBOIndices,
		VBOCount,
	};

	void InitAttribute(const aiMesh* mesh, SurfaceAttribute& attribute);
	void InitMeshes(const aiScene* scene, Material* materials);
	bool InitFromScene(const aiScene* scene, const std::string& path);
	bool InitMaterials(const aiScene* scene, const std::string& path, Material* materials);
	void InitMeshAttributes(const aiScene* scene, unsigned numVertices, unsigned numIndices);

	void Clear();
	void UpdateGLBuffers(const SurfaceAttribute& attribute);

private:
	GLuint vao_;
	GLuint vbos_[VBOCount];

	std::vector<Mesh> meshes_;
};
