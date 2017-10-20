#pragma once

#include <map>
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

class SequenceIDCreater;

class SurfaceInternal : public ISurface, public ObjectInternal {
	DEFINE_FACTORY_METHOD(Surface)

public:
	SurfaceInternal();
	~SurfaceInternal();

public:
	virtual bool Load(const std::string& path);

	virtual void SetAttribute(const SurfaceAttribute& value);
	virtual void AddMesh(Mesh mesh);
	virtual int GetMeshCount();
	virtual Mesh GetMesh(int index);

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

	struct Bone {
		glm::mat4 localToBoneSpaceMatrix;
		glm::mat4 world;
	};

	void InitAttribute(const aiMesh* aimesh, int nm, SurfaceAttribute& attribute);
	void InitBoneAttribute(const aiMesh* aimesh, int nm, SurfaceAttribute &attribute);

	void InitMeshes(const aiScene* scene, MaterialTextures* textures);
	bool InitFromScene(const aiScene* scene, const std::string& path);
	void InitTextures(const aiScene* scene, const std::string& path, MaterialTextures* textures);
	void InitMeshAttributes(const aiScene* scene, unsigned numVertices, unsigned numIndexes);

	void Clear();
	void UpdateGLBuffers(const SurfaceAttribute& attribute);

private:
	GLuint vao_;
	GLuint vbos_[VBOCount];
	std::vector<Mesh> meshes_;

	typedef std::map<std::string, unsigned> BoneMap;

	BoneMap map_;
	std::vector<Bone> bones_;
};
