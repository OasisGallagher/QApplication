#pragma once

#include <map>
#include <vector>
#include <gl/glew.h>
#include <glm/glm.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>

#include "defs.h"

#pragma warning(push)
#pragma warning(disable:4251)

class Texture2D;

class GRAPHICS_EXPORT Mesh {
public:
	Mesh();
	~Mesh();

public:
	bool Load(const std::string& path);
	void Render(GLenum mode = GL_TRIANGLES);

private:
	enum {
		VBOPositions,
		VBOUVs,
		VBONormals,
		VBOIndices,
		VBOCount,
	};

	struct MeshData {
		std::vector<glm::vec3> positions;
		std::vector<glm::vec3> normals;
		std::vector<glm::vec2> uvs;
		std::vector<unsigned> indices;
	};

	struct MeshEntry {
		unsigned numIndices;
		unsigned baseVertex;
		unsigned baseIndex;
		unsigned materialIndex;
	};

	void InitMesh(const aiMesh* mesh, MeshData& data);
	bool InitFromScene(const aiScene* scene, const std::string& path);
	bool InitMaterials(const aiScene* scene, const std::string& path);

	void Clear();

private:
	GLuint vao_;
	GLuint vbos_[VBOCount];

	std::vector<MeshEntry> entries_;
	std::vector<Texture2D*> textures_;
};

class SkinnedMesh {
public:
	SkinnedMesh();
	~SkinnedMesh();

public:
	bool Load(const std::string& path);
	void Render();

	unsigned GetBoneCount() const;
	void GetBoneTransform(float time, std::vector<glm::mat4>& transforms);

private:
	enum {
		BoneCountPerVertex = 4
	};

	enum {
		VBOPositions,
		VBOUVs,
		VBONormals,
		VBOBones,
		VBOIndices,
		VBOCount,
	};

	enum {
		LocationPosition,
		LocationUV,
		LocationNormal,
		LocationBoneID,
		LocationBoneWeight,
	};

	struct BoneInfo {
		glm::mat4 boneOffset;
		glm::mat4 finalTransform;
	};

	struct VertexBoneData {
		unsigned ids[BoneCountPerVertex];
		float weights[BoneCountPerVertex];

		void AddBoneData(unsigned boneID, float weight);
	};

	struct SkinnedMeshData {
		std::vector<glm::vec3> positions;
		std::vector<glm::vec3> normals;
		std::vector<glm::vec2> uvs;
		std::vector<VertexBoneData> bones;
		std::vector<unsigned> indices;
	};

	struct SkinnedMeshEntry {
		unsigned numIndices;
		unsigned baseVertex;
		unsigned baseIndex;
		unsigned materialIndex;
	};

	void CalculateInterpolatedScaling(aiVector3D& answer, float time, const aiNodeAnim* node);
	void CalculateInterpolatedRotation(aiQuaternion& answer, float time, const aiNodeAnim* node);
	void CalculateInterpolatedPosition(aiVector3D& answer, float time, const aiNodeAnim* node);

	unsigned FindScaling(float time, const aiNodeAnim* node);
	unsigned FindRotation(float time, const aiNodeAnim* node);
	unsigned FindPosition(float time, const aiNodeAnim* node);

	const aiNodeAnim* FindNodeAnim(const aiAnimation* anim, const std::string& name);
	void ReadNodeHeirarchy(float time, const aiNode* node, const glm::mat4& parentTransform);

	bool InitFromScene(const aiScene* scene, const std::string& path);
	void InitMesh(unsigned index, const aiMesh* mesh, SkinnedMeshData& data);
	void LoadBones(unsigned meshIndex, const aiMesh* mesh, std::vector<VertexBoneData>& bones);
	bool InitMaterials(const aiScene* scene, const std::string& path);

	void Clear();

private:
	GLuint vao_;
	GLuint vbos_[VBOCount];

	std::vector<SkinnedMeshEntry> entries_;
	std::vector<Texture2D*> textures_;

	typedef std::map<std::string, unsigned> BoneMap;
	BoneMap boneMap_;

	unsigned boneCount_;
	std::vector<BoneInfo> boneInfos_;

	glm::mat4 globalInverseTransform_;

	const aiScene* scene_;
	Assimp::Importer importer_;
};

#pragma warning(pop)
