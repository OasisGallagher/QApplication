#pragma once
#include <vector>
#include <gl/glew.h>

#include "mesh.h"
#include "object.h"

enum {
	MaxBoneCount = 4,
};

struct BoneAttribute {
	unsigned indexes[MaxBoneCount];
	float weights[MaxBoneCount];
};

struct SurfaceAttribute {
	std::vector<glm::vec3> positions;
	std::vector<glm::vec3> normals;
	std::vector<glm::vec2> texCoords;
	std::vector<glm::vec3> tangents;
	std::vector<BoneAttribute> bones;
	std::vector<unsigned> indexes;
};

class ENGINE_EXPORT ISurface : virtual public IObject {
public:
	virtual bool Load(const std::string& path) = 0;

	virtual void SetAttribute(const SurfaceAttribute& value) = 0;
	virtual void AddMesh(Mesh mesh) = 0;
	virtual int GetMeshCount() = 0;
	virtual Mesh GetMesh(int index) = 0;

	virtual void Bind() = 0;
	virtual void Unbind() = 0;
};

typedef std::shared_ptr<ISurface> Surface;
