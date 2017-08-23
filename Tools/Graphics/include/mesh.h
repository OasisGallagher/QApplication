#pragma once

#include <map>
#include <vector>
#include <gl/glew.h>
#include <glm/glm.hpp>

#include "defs.h"

class Texture2D;

struct aiMesh;
struct aiScene;
struct aiNodeAnim;
struct aiAnimation;
struct aiNode;

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
