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
	bool InitMaterials(const aiScene* scene, const std::string& path, Material** materials);

	void Clear();

private:
	GLuint vao_;
	GLuint vbos_[VBOCount];

	std::vector<Mesh> meshes_;
};
