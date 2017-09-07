#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <glm/gtc/matrix_transform.hpp>

#include "defs.h"
#include "debug.h"
#include "surface.h"
#include "texture.h"
#include "material.h"
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

SurfacePrivate::SurfacePrivate() {
	vao_ = 0;
	memset(vbos_, 0, COUNT_OF(vbos_));
}

SurfacePrivate::~SurfacePrivate() {
	Clear();
}

void SurfacePrivate::Clear() {
	glDeleteVertexArrays(1, &vao_);
	glDeleteVertexArrays(COUNT_OF(vbos_), vbos_);
}

bool SurfacePrivate::Load(const std::string& path) {
	Clear();

	glGenVertexArrays(1, &vao_);
	glBindVertexArray(vao_);

	glGenBuffers(COUNT_OF(vbos_), vbos_);

	Assimp::Importer importer;
	unsigned flags = aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace;
	std::string fpath = "resources/" + path;
	const aiScene* scene = importer.ReadFile(fpath.c_str(), flags);

	AssertX(scene != nullptr, "failed to read file " + fpath + ": " + importer.GetErrorString());

	bool result = InitFromScene(scene, fpath);

	glBindVertexArray(0);

	return result;
}

bool SurfacePrivate::InitFromScene(const aiScene* scene, const std::string& path) {
	Material* materials = new Material[scene->mNumMaterials];

	if (!InitMaterials(scene, path, materials)) {
		return false;
	}

	meshes_.resize(scene->mNumMeshes);

	Attribute attribute;
	unsigned numVertices = 0, numIndices = 0;

	// Count the numer of vertices and indices.
	for (unsigned i = 0; i < meshes_.size(); ++i) {
		meshes_[i].numIndices = scene->mMeshes[i]->mNumFaces * 3;
		meshes_[i].baseVertex = numVertices;
		meshes_[i].baseIndex = numIndices;

		if (scene->mMeshes[i]->mMaterialIndex < scene->mNumMaterials) {
			meshes_[i].material = materials[scene->mMeshes[i]->mMaterialIndex];
		}

		numVertices += scene->mMeshes[i]->mNumVertices;
		numIndices += meshes_[i].numIndices;
	}

	attribute.positions.reserve(numVertices);
	attribute.normals.reserve(numVertices);
	attribute.uvs.reserve(numVertices);
	attribute.indices.reserve(numIndices);

	for (unsigned i = 0; i < meshes_.size(); ++i) {
		InitAttribute(scene->mMeshes[i], attribute);
	}

	glBindBuffer(GL_ARRAY_BUFFER, vbos_[VBOPositions]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * attribute.positions.size(), &attribute.positions[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(VBOPositions);
	glVertexAttribPointer(VBOPositions, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

	glBindBuffer(GL_ARRAY_BUFFER, vbos_[VBOUVs]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * attribute.uvs.size(), &attribute.uvs[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(VBOUVs);
	glVertexAttribPointer(VBOUVs, 2, GL_FLOAT, GL_FALSE, 0, nullptr);

	glBindBuffer(GL_ARRAY_BUFFER, vbos_[VBONormals]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * attribute.normals.size(), &attribute.normals[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(VBONormals);
	glVertexAttribPointer(VBONormals, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbos_[VBOIndices]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned) * attribute.indices.size(), &attribute.indices[0], GL_STATIC_DRAW);

	return true;
}

void SurfacePrivate::InitAttribute(const aiMesh* mesh, Attribute& attribute) {
	const aiVector3D zero(0);
	for (unsigned i = 0; i < mesh->mNumVertices; ++i) {
		const aiVector3D* pos = &mesh->mVertices[i];
		const aiVector3D* normal = &mesh->mNormals[i];
		const aiVector3D* texCoord = mesh->HasTextureCoords(0) ? &(mesh->mTextureCoords[0][i]) : &zero;
		const aiVector3D* tangent = (mesh->mTangents != nullptr) ? &mesh->mTangents[i] : &zero;

		attribute.positions.push_back(glm::vec3(pos->x, pos->y, pos->z));
		attribute.normals.push_back(glm::vec3(normal->x, normal->y, normal->z));
		attribute.uvs.push_back(glm::vec2(texCoord->x, texCoord->y));
	}

	for (unsigned i = 0; i < mesh->mNumFaces; ++i) {
		const aiFace& face = mesh->mFaces[i];
		AssertX(face.mNumIndices == 3, "invalid index count");
		attribute.indices.push_back(face.mIndices[0]);
		attribute.indices.push_back(face.mIndices[1]);
		attribute.indices.push_back(face.mIndices[2]);
	}
}

bool SurfacePrivate::InitMaterials(const aiScene* scene, const std::string& path, Material* materials) {
	std::string dir = Utility::GetDirectoryPath(path);

	bool success = true;
	for (unsigned i = 0; i < scene->mNumMaterials; ++i) {
		const aiMaterial* mat = scene->mMaterials[i];
		
		if (mat->GetTextureCount(aiTextureType_DIFFUSE) > 0) {
			aiString dpath;
			if (mat->GetTexture(aiTextureType_DIFFUSE, 0, &dpath) != AI_SUCCESS) {
				continue;
			}

			std::string fullPath = "textures/";
			fullPath += dpath.data;
			
			Texture2D texture = Texture2D::Create();
			
			if (!texture->Load(fullPath)) {
				success = false;
				continue;
			}

			materials[i] = Material::Create();
			materials[i]->SetDiffuseTexture(texture);
		}
	}

	return success;
}

void SurfacePrivate::Render(GLenum mode) {
	glBindVertexArray(vao_);

	AssertX(mode == GL_TRIANGLES || mode == GL_PATCHES, "invalid mode");

	for (unsigned i = 0; i < meshes_.size(); ++i) {
		
		Texture diffuse = meshes_[i].material->GetDiffuseTexture();
		
		if (diffuse) {
			diffuse->Bind(Globals::ColorTexture);
		}

		glDrawElementsBaseVertex(mode, meshes_[i].numIndices, GL_UNSIGNED_INT,
								 (void*)(sizeof(unsigned)* meshes_[i].baseIndex), meshes_[i].baseVertex);

	}

	glBindVertexArray(0);
}
