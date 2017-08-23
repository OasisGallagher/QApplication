#pragma once
#include <vector>
#include <glm/glm.hpp>

class TextLoader {
public:
	static bool Load(const std::string& file, std::string& text);
	static bool Load(const std::string& file, std::vector<std::string>& lines);
};

struct ModelInfo {
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec2> uvs;
	std::vector<glm::vec3> normals;
};

class ModelLoader {
public:
	static bool Load(const std::string& path, ModelInfo& info);
	static bool Load2(const std::string& path, ModelInfo& info);

private:
	static bool LoadBlenderObj(const std::string& path, ModelInfo& info);
};

class VBOIndexer {
	struct VBOBuffer {
		glm::vec3 vertex;
		glm::vec2 uv;
		glm::vec3 normal;

		bool operator < (const VBOBuffer& other) const {
			return memcmp(this, &other, sizeof(other)) < 0;
		}
	};

	struct NormalMappingVBOBuffer {
		glm::vec3 vertex;
		glm::vec2 uv;
		glm::vec3 normal;

		glm::vec3 tangent;
		glm::vec3 bitangent;

		bool operator < (const NormalMappingVBOBuffer& other) const {
			return memcmp(this, &other, sizeof(other)) < 0;
		}
	};
public:
	static void Index(ModelInfo& packed, std::vector<unsigned>& indices, const ModelInfo& info);
	static void Index(ModelInfo& packed, std::vector<glm::vec3>& packedTangents, std::vector<glm::vec3>& packedBitangents, std::vector<unsigned>& indices, const ModelInfo& info, const std::vector<glm::vec3>& tangents, const std::vector<glm::vec3>& bitangents);
};

class TBNParser {
public:
	static void Parse(std::vector<glm::vec3>& tangents, std::vector<glm::vec3>& bitangents, const ModelInfo& info);
};
