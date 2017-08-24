#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <glm/gtc/matrix_transform.hpp>

#include "defs.h"
#include "mesh.h"
#include "debug.h"
#include "texture.h"
#include "utilities.h"

static void aiMaterixToGlm(glm::mat4& answer, const aiMatrix4x4& mat) {
	answer = glm::mat4(
		mat.a1, mat.b1, mat.c1, mat.d1,
		mat.a2, mat.b2, mat.c2, mat.d2,
		mat.a3, mat.b3, mat.c3, mat.d3,
		mat.a4, mat.b4, mat.c4, mat.d4
		);
}

static void aiMaterixToGlm(glm::mat4& answer, const aiMatrix3x3& mat) {
	answer = glm::mat4(
		mat.a1, mat.b1, mat.c1, 0,
		mat.a2, mat.b2, mat.c2, 0,
		mat.a3, mat.b3, mat.c3, 0,
		0, 0, 0, 1
		);
}

Mesh::Mesh() {
	vao_ = 0;
	memset(vbos_, 0, COUNT_OF(vbos_));
}

Mesh::~Mesh() {
	Clear();
}

void Mesh::Clear() {
	for (unsigned i = 0; i < textures_.size(); ++i) {
		delete textures_[i];
	}

	textures_.clear();
	glDeleteVertexArrays(1, &vao_);
	glDeleteVertexArrays(COUNT_OF(vbos_), vbos_);
}

bool Mesh::Load(const std::string& path) {
	Clear();

	glGenVertexArrays(1, &vao_);
	glBindVertexArray(vao_);

	glGenBuffers(COUNT_OF(vbos_), vbos_);

	Assimp::Importer importer;
	unsigned flags = aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace;
	const aiScene* scene = importer.ReadFile(path.c_str(), flags);

	Assert(scene != nullptr, "failed to read file " + path + ": " + importer.GetErrorString());

	bool result = InitFromScene(scene, path);

	glBindVertexArray(0);

	return result;
}

bool Mesh::InitFromScene(const aiScene* scene, const std::string& path) {
	entries_.resize(scene->mNumMeshes);
	textures_.resize(scene->mNumMaterials);

	MeshData data;
	unsigned numVertices = 0, numIndices = 0;

	// Count the numer of vertices and indices.
	for (unsigned i = 0; i < entries_.size(); ++i) {
		entries_[i].materialIndex = scene->mMeshes[i]->mMaterialIndex;
		entries_[i].numIndices = scene->mMeshes[i]->mNumFaces * 3;
		entries_[i].baseVertex = numVertices;
		entries_[i].baseIndex = numIndices;

		numVertices += scene->mMeshes[i]->mNumVertices;
		numIndices += entries_[i].numIndices;
	}

	data.positions.reserve(numVertices);
	data.normals.reserve(numVertices);
	data.uvs.reserve(numVertices);
	data.indices.reserve(numIndices);

	for (unsigned i = 0; i < entries_.size(); ++i) {
		InitMesh(scene->mMeshes[i], data);
	}

	if (!InitMaterials(scene, path)) {
		return false;
	}

	glBindBuffer(GL_ARRAY_BUFFER, vbos_[VBOPositions]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * data.positions.size(), &data.positions[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(VBOPositions);
	glVertexAttribPointer(VBOPositions, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

	glBindBuffer(GL_ARRAY_BUFFER, vbos_[VBOUVs]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * data.uvs.size(), &data.uvs[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(VBOUVs);
	glVertexAttribPointer(VBOUVs, 2, GL_FLOAT, GL_FALSE, 0, nullptr);

	glBindBuffer(GL_ARRAY_BUFFER, vbos_[VBONormals]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * data.normals.size(), &data.normals[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(VBONormals);
	glVertexAttribPointer(VBONormals, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbos_[VBOIndices]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned) * data.indices.size(), &data.indices[0], GL_STATIC_DRAW);

	return true;
}

void Mesh::InitMesh(const aiMesh* mesh, MeshData& data) {
	const aiVector3D zero(0);
	for (unsigned i = 0; i < mesh->mNumVertices; ++i) {
		const aiVector3D* pos = &mesh->mVertices[i];
		const aiVector3D* normal = &mesh->mNormals[i];
		const aiVector3D* texCoord = mesh->HasTextureCoords(0) ? &(mesh->mTextureCoords[0][i]) : &zero;
		const aiVector3D* tangent = (mesh->mTangents != nullptr) ? &mesh->mTangents[i] : &zero;

		data.positions.push_back(glm::vec3(pos->x, pos->y, pos->z));
		data.normals.push_back(glm::vec3(normal->x, normal->y, normal->z));
		data.uvs.push_back(glm::vec2(texCoord->x, texCoord->y));
	}

	for (unsigned i = 0; i < mesh->mNumFaces; ++i) {
		const aiFace& face = mesh->mFaces[i];
		Assert(face.mNumIndices == 3, "invalid index count");
		data.indices.push_back(face.mIndices[0]);
		data.indices.push_back(face.mIndices[1]);
		data.indices.push_back(face.mIndices[2]);
	}
}

bool Mesh::InitMaterials(const aiScene* scene, const std::string& path) {
	std::string dir = Utility::GetDirectoryPath(path);

	bool success = true;
	for (unsigned i = 0; i < scene->mNumMaterials; ++i) {
		const aiMaterial* material = scene->mMaterials[i];
		textures_[i] = nullptr;

		if (material->GetTextureCount(aiTextureType_DIFFUSE) > 0) {
			aiString dpath;
			if (material->GetTexture(aiTextureType_DIFFUSE, 0, &dpath) != AI_SUCCESS) {
				continue;
			}

			std::string fullPath = "textures/";
			fullPath += dpath.data;
			textures_[i] = new Texture2D;
			if (!textures_[i]->Load(fullPath)) {
				success = false;
				delete textures_[i];
				textures_[i] = nullptr;
			}
		}
	}

	return success;
}

void Mesh::Render(GLenum mode) {
	glBindVertexArray(vao_);

	Assert(mode == GL_TRIANGLES || mode == GL_PATCHES, "invalid mode");

	for (unsigned i = 0; i < entries_.size(); ++i) {
		unsigned materilIndex = entries_[i].materialIndex;
		Assert(materilIndex < textures_.size(), "invalid materialIndex");

		if (textures_[materilIndex] != nullptr) {
			textures_[materilIndex]->Bind(Globals::ColorTexture);
		}

		glDrawElementsBaseVertex(mode, entries_[i].numIndices, GL_UNSIGNED_INT,
								 (void*)(sizeof(unsigned)* entries_[i].baseIndex), entries_[i].baseVertex);

	}

	glBindVertexArray(0);
}
