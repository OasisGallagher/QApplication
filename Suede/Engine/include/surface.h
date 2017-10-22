#pragma once
#include <vector>
#include <gl/glew.h>

#include "mesh.h"
#include "object.h"

struct BlendAttribute {
	enum {
		Quality = 4,
	};

	unsigned indexes[Quality];
	float weights[Quality];
};

struct SurfaceAttribute {
	std::vector<glm::vec3> positions;
	std::vector<glm::vec3> normals;
	std::vector<glm::vec2> texCoords;
	std::vector<glm::vec3> tangents;
	std::vector<BlendAttribute> blendAttrs;
	std::vector<unsigned> indexes;
};

class ENGINE_EXPORT ISurface : virtual public IObject {
public:
	virtual void SetAttribute(const SurfaceAttribute& value) = 0;
	virtual void AddMesh(Mesh mesh) = 0;
	virtual int GetMeshCount() = 0;
	virtual Mesh GetMesh(int index) = 0;

	virtual void Bind() = 0;
	virtual void Unbind() = 0;
};

typedef std::shared_ptr<ISurface> Surface;
