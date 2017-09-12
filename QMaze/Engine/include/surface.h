#pragma once
#include <vector>
#include <gl/glew.h>

#include "object.h"

struct SurfaceAttribute {
	std::vector<glm::vec3> positions;
	std::vector<glm::vec3> normals;
	std::vector<glm::vec2> uvs;
	std::vector<glm::vec3> tangents;
	std::vector<unsigned> indices;
};

class ENGINE_EXPORT ISurface : virtual public IObject {
public:
	virtual bool Load(const std::string& path) = 0;

	virtual void SetAttribute(const SurfaceAttribute& value) = 0;
	virtual void AddMesh(Mesh mesh) = 0;

	virtual void Bind() = 0;
	virtual void Unbind() = 0;
};

typedef smart_ptr<ISurface> Surface;
