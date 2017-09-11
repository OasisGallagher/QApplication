#pragma once

#include <vector>
#include <gl/glew.h>
#include <glm/glm.hpp>

#include "material.h"
#include "surface.h"
#include "internal/base/objectinternal.h"

struct aiNode;
struct aiMesh;
struct aiScene;
struct aiNodeAnim;
struct aiAnimation;

class SurfaceInternal : public Surface, public ObjectInternal {
public:
	SurfaceInternal();
	~SurfaceInternal();

public:
	virtual bool Load(const std::string& path);
	virtual void SetAttribute(const SurfaceAttribute& value);
	virtual void AddMesh(Mesh* mesh);

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

	void InitMeshes(const aiScene* scene);
	void InitAttribute(const aiMesh* mesh);
	bool InitFromScene(const aiScene* scene, const std::string& path);
	bool InitMaterials(const aiScene* scene, const std::string& path);
	void InitMeshAttributes(const aiScene* scene, unsigned numVertices, unsigned numIndices);

	void Clear();

private:
	GLuint vao_;
	GLuint vbos_[VBOCount];

	SurfaceAttribute attribute_;

	std::vector<Mesh*> meshes_;
	std::vector<Material*> sceneMaterials_;
};
