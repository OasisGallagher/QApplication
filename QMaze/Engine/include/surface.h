#pragma once

#include <map>
#include <vector>
#include <gl/glew.h>
#include <glm/glm.hpp>

#include "defs.h"

class Material;

struct aiMesh;
struct aiScene;
struct aiNodeAnim;
struct aiAnimation;
struct aiNode;

class ENGINE_EXPORT Surface {
public:
	Surface();
	~Surface();

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

	struct Attribute {
		std::vector<glm::vec3> positions;
		std::vector<glm::vec3> normals;
		std::vector<glm::vec2> uvs;
		std::vector<unsigned> indices;
	};

	struct Mesh {
		unsigned numIndices;
		unsigned baseVertex;
		unsigned baseIndex;

		Material* material;
	};

	void InitAttribute(const aiMesh* mesh, Attribute& attribute);
	bool InitFromScene(const aiScene* scene, const std::string& path);
	bool InitMaterials(const aiScene* scene, const std::string& path);

	void Clear();

private:
	GLuint vao_;
	GLuint vbos_[VBOCount];

	std::vector<Mesh> meshes_;
	std::vector<Material*> materials_;
};
