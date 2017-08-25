#include <map>
#include <fstream>

#include <assimp/scene.h>
#include <assimp/importer.hpp>
#include <assimp/postprocess.h>

#include "debug.h"
#include "loader.h"

#include "debug.h"

#define HAS_NORMAL_INDEX

bool TextLoader::Load(const std::string& file, std::string& text) {
	std::ifstream ifs(file, std::ios::in);
	if (!ifs) {
		Debug::LogError("failed to open file " + file + ".");
		return false;
	}

	text.clear();
	const char* seperator = "";

	std::string line;
	for (; getline(ifs, line);) {
		if (!line.empty()) {
			text += seperator;
			text += line;
			seperator = "\n";
		}
	}

	ifs.close();

	return true;
}

bool TextLoader::Load(const std::string& file, std::vector<std::string>& lines) {
	std::ifstream ifs(file, std::ios::in);
	if (!ifs) {
		Debug::LogError("failed to open file " + file + ".");
		return false;
	}

	lines.clear();

	std::string line;
	for (; getline(ifs, line);) {
		if (!line.empty()) {
			lines.push_back(line);
		}
	}

	ifs.close();

	return true;
}

bool ModelLoader::Load(const std::string& path, ModelInfo& info){
	std::string::size_type i = path.rfind('.');
	if (i == std::string::npos) {
		Debug::LogError("invalid file path " + path + ".");
		return false;
	}

	const char* ptr = path.c_str() + i + 1;
	if (strcmp("obj", ptr) == 0) {
		return LoadBlenderObj(path, info);
	}

	Debug::LogError("invalid postfix " + path.substr(i) + ".");
	return false;
}

bool ModelLoader::Load2(const std::string& path, ModelInfo& info) {
	/*Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(path.c_str(), 0);

	if (scene == nullptr) {
		Debug::LogError(importer.GetErrorString());
		return false;
	}

	const aiMesh* mesh = scene->mMeshes[0];
	info.vertices.reserve(mesh->mNumVertices);
	for (unsigned i = 0; i < mesh->mNumVertices; ++i) {
		aiVector3D UVW = mesh->mTextureCoords[0][i];
		info.uvs.push_back(glm::vec2(UVW.x, UVW.y));
	}

	info.normals.reserve(mesh->mNumVertices);
	for (unsigned int i = 0; i < mesh->mNumVertices; i++){
		aiVector3D n = mesh->mNormals[i];
		info.normals.push_back(glm::vec3(n.x, n.y, n.z));
	}
	*/
	// Fill face indices
// 	info.indices.reserve(3 * mesh->mNumFaces);
// 	for (unsigned int i = 0; i < mesh->mNumFaces; i++){
// 		// Assume the model has only triangles.
// 		info.indices.push_back(mesh->mFaces[i].mIndices[0]);
// 		indices.push_back(mesh->mFaces[i].mIndices[1]);
// 		indices.push_back(mesh->mFaces[i].mIndices[2]);
// 	}

	return false;
}

bool ModelLoader::LoadBlenderObj(const std::string& path, ModelInfo& info) {
	FILE* file = fopen(path.c_str(), "r");
	if (file == nullptr) {
		Debug::LogError("invalid file path " + path + ".");
		return false;
	}

	glm::vec2 uv;
	glm::vec3 vertex, normal;

	std::vector<glm::vec2> uvs;
	std::vector<glm::vec3> vertices, normals;

	unsigned vertexIndex[3], uvIndex[3], normalIndex[3];
	std::vector<unsigned> vertexIndices, uvIndices, normalIndices;

	char header[128];
	char dummy[512];
	
	for (;!feof(file);) {
		fscanf(file, "%s", header);

		if (strcmp(header, "v") == 0) {
			fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
			vertices.push_back(vertex);
		}
		else if (strcmp(header, "vt") == 0) {
			fscanf(file, "%f %f\n", &uv.x, &uv.y);
			uv.y = -uv.y;
			uvs.push_back(uv);
		}
		else if (strcmp(header, "vn") == 0) {
			fscanf(file, "%f %f %f", &normal.x, &normal.y, &normal.z);
			normals.push_back(normal);
		}
		else if (strcmp(header, "f") == 0) {
#ifdef HAS_NORMAL_INDEX
			fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n",
				&vertexIndex[0], &uvIndex[0], &normalIndex[0],
				&vertexIndex[1], &uvIndex[1], &normalIndex[1],
				&vertexIndex[2], &uvIndex[2], &normalIndex[2]
				);
#else

			fscanf(file, "%d/%d %d/%d %d/%d\n",
				   &vertexIndex[0], &uvIndex[0], //&normalIndex[0],
				   &vertexIndex[1], &uvIndex[1], //&normalIndex[1],
				   &vertexIndex[2], &uvIndex[2]//, &normalIndex[2]
				   );
#endif // HAS_NORMAL_INDEX
			vertexIndices.insert(vertexIndices.end(), vertexIndex, vertexIndex + 3);
			uvIndices.insert(uvIndices.end(), uvIndex, uvIndex + 3);
			normalIndices.insert(normalIndices.end(), normalIndex, normalIndex + 3);
		}
		else {
			fgets(dummy, sizeof(dummy), file);
		}
	}

	for (unsigned i = 0; i < vertexIndices.size(); ++i) {
		unsigned vi = vertexIndices[i];
		unsigned ui = uvIndices[i];
		unsigned ni = normalIndices[i];

		info.vertices.push_back(vertices[vi - 1]);
		if (ui - 1 < uvs.size()) {
			info.uvs.push_back(uvs[ui - 1]);
		}

		if (ui - 1 < normals.size()) {
			info.normals.push_back(normals[ni - 1]);
		}
	}

	return true;
}

void VBOIndexer::Index(ModelInfo& packed, std::vector<unsigned>& indices, const ModelInfo& info) {
	ModelInfo newInfo;
	typedef std::map<VBOBuffer, unsigned> Container;
	Container dict;
	unsigned index = 0;
	unsigned size = info.vertices.size();

	for (unsigned i = 0; i < size; ++i) {
		VBOBuffer buffer = { info.vertices[i], info.uvs[i], info.normals[i] };
		Container::iterator pos = dict.find(buffer);
		if (pos == dict.end()) {
			dict.insert(std::make_pair(buffer, index));

			newInfo.vertices.push_back(buffer.vertex);
			newInfo.uvs.push_back(buffer.uv);
			newInfo.normals.push_back(buffer.normal);

			indices.push_back(index);
			++index;
		}
		else {
			indices.push_back(pos->second);
		}
	}

	packed = newInfo;
}

void VBOIndexer::Index(ModelInfo& packed, std::vector<glm::vec3>& packedTangents, std::vector<glm::vec3>& packedBitangents, std::vector<unsigned>& indices, const ModelInfo& info, const std::vector<glm::vec3>& tangents, const std::vector<glm::vec3>& bitangents){
	ModelInfo newInfo;
	std::vector<glm::vec3> newTangents, newBitangents;

	typedef std::map<NormalMappingVBOBuffer, unsigned> Container;
	Container dict;
	unsigned index = 0;
	unsigned size = info.vertices.size();

	for (unsigned i = 0; i < size; ++i) {
		NormalMappingVBOBuffer buffer = { info.vertices[i], info.uvs[i], info.normals[i], tangents[i], bitangents[i] };
		Container::iterator pos = dict.find(buffer);
		if (pos == dict.end()) {
			dict.insert(std::make_pair(buffer, index));

			newInfo.vertices.push_back(buffer.vertex);
			newInfo.uvs.push_back(buffer.uv);
			newInfo.normals.push_back(buffer.normal);

			newTangents.push_back(buffer.tangent);
			newBitangents.push_back(buffer.bitangent);

			indices.push_back(index);
			++index;
		}
		else {
			indices.push_back(pos->second);
			newTangents[pos->second] += tangents[i];
			newBitangents[pos->second] += bitangents[i];
		}
	}

	packed = newInfo;
	packedTangents = newTangents;
	packedBitangents = newBitangents;
}

void TBNParser::Parse(std::vector<glm::vec3>& tangents, std::vector<glm::vec3>& bitangents, const ModelInfo& info) {
	for (unsigned i = 0; i < info.vertices.size(); i += 3) {
		const glm::vec3& p0 = info.vertices[i];
		const glm::vec3& p1 = info.vertices[i + 1];
		const glm::vec3& p2 = info.vertices[i + 2];

		glm::vec3 deltaPos1 = p1 - p0;
		glm::vec3 deltaPos2 = p2 - p0;

		const glm::vec2& uv0 = info.uvs[0];
		const glm::vec2& uv1 = info.uvs[1];
		const glm::vec2& uv2 = info.uvs[2];

		glm::vec2 deltaUV1 = uv1 - uv0;
		glm::vec2 deltaUV2 = uv2 - uv0;

		// deltaPos1 = deltaUV1.x * T + deltaUV1.y * B
		// deltaPos2 = deltaUV2.x * T + deltaUV2.y * B
		// B = (deltaPos1 * deltaUV2.x - deltaPos2 * deltaUV1.x) / (deltaUV1.y * deltaUV2.x - deltaUV1.x * deltaUV2.y)
		// T = (deltaPos1 * deltaUV2.y - deltaPos2 * deltaUV1.y) / (deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x)

		float r = 1.f / (deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x);
		glm::vec3 T = (deltaPos1 * deltaUV2.y - deltaPos2 * deltaUV1.y) * r;
		glm::vec3 B = (deltaPos2 * deltaUV1.x - deltaPos1 * deltaUV2.x) * r;

		tangents.push_back(T);
		tangents.push_back(T);
		tangents.push_back(T);

		bitangents.push_back(B);
		bitangents.push_back(B);
		bitangents.push_back(B);
	}

	for (unsigned i = 0; i < info.vertices.size(); ++i){
		glm::vec3& t = tangents[i];
		glm::vec3& b = bitangents[i];
		const glm::vec3& n = info.normals[i];

		t = glm::normalize(t - n * glm::dot(n, t));

		if (glm::dot(glm::cross(n, t), b) < 0.f){
			t = -t;
		}
	}
}
