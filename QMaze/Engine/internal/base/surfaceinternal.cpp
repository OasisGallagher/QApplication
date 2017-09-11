#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <glm/gtc/matrix_transform.hpp>

#include "tools/debug.h"
#include "tools/mathf.h"
#include "tools/path.h"
#include "surfaceinternal.h"
#include "internal/memory/memory.h"
#include "internal/memory/factory.h"
#include "internal/base/meshinternal.h"
#include "internal/base/textureinternal.h"
#include "internal/base/materialinternal.h"

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

SurfaceInternal::SurfaceInternal() 
	: ObjectInternal(ObjectTypeSurface) {
	vao_ = 0;
	memset(vbos_, 0, CountOf(vbos_));
}

SurfaceInternal::~SurfaceInternal() {
	Clear();
}

void SurfaceInternal::Clear() {
	glDeleteVertexArrays(1, &vao_);
	vao_ = 0;

	glDeleteVertexArrays(CountOf(vbos_), vbos_);
	std::fill(vbos_, vbos_ + CountOf(vbos_), 0);

	attribute_.positions.clear();
	attribute_.normals.clear();
	attribute_.uvs.clear();
	attribute_.tangents.clear();
	attribute_.indices.clear();

	std::for_each(sceneMaterials_.begin(), sceneMaterials_.end(), Factory::Release);
	sceneMaterials_.clear();

	std::for_each(meshes_.begin(), meshes_.end(), Factory::Release);
	meshes_.clear();
}

void SurfaceInternal::SetAttribute(const SurfaceAttribute& value) {
	Clear();
	attribute_ = value;
}

bool SurfaceInternal::Load(const std::string& path) {
	Clear();

	glGenVertexArrays(1, &vao_);
	glBindVertexArray(vao_);

	glGenBuffers(CountOf(vbos_), vbos_);

	Assimp::Importer importer;
	unsigned flags = aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace;
	std::string fpath = "resources/" + path;
	const aiScene* scene = importer.ReadFile(fpath.c_str(), flags);

	AssertX(scene != nullptr, "failed to read file " + fpath + ": " + importer.GetErrorString());

	bool result = InitFromScene(scene, fpath);

	glBindVertexArray(0);

	return result;
}

bool SurfaceInternal::InitFromScene(const aiScene* scene, const std::string& path) {
	if (!InitMaterials(scene, path)) {
		Debug::LogError("material incomplete.");
	}

	InitMeshes(scene);

	return true;
}

void SurfaceInternal::InitMeshes(const aiScene* scene) {
	meshes_.resize(scene->mNumMeshes);

	unsigned vertexCount = 0, indexCount = 0;

	for (unsigned i = 0; i < scene->mNumMeshes; ++i) {
		Mesh* mesh = meshes_[i] = Memory::Create<MeshInternal>();
		mesh->SetTriangles(scene->mMeshes[i]->mNumFaces * 3, vertexCount, indexCount);

		if (scene->mMeshes[i]->mMaterialIndex < scene->mNumMaterials) {
			// ???
			//meshes_[i]->SetMaterial(materials_[scene->mMeshes[i]->mMaterialIndex]);
		}

		vertexCount += scene->mMeshes[i]->mNumVertices;
		indexCount += scene->mMeshes[i]->mNumFaces * 3;
	}

	InitMeshAttributes(scene, vertexCount, indexCount);
}

void SurfaceInternal::InitMeshAttributes(const aiScene* scene, unsigned numVertices, unsigned numIndices) {
	attribute_.positions.reserve(numVertices);
	attribute_.normals.reserve(numVertices);
	attribute_.uvs.reserve(numVertices);
	attribute_.indices.reserve(numIndices);

	for (unsigned i = 0; i < scene->mNumMeshes; ++i) {
		InitAttribute(scene->mMeshes[i]);
	}

	glBindBuffer(GL_ARRAY_BUFFER, vbos_[VBOPositions]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * attribute_.positions.size(), &attribute_.positions[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(VBOPositions);
	glVertexAttribPointer(VBOPositions, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

	glBindBuffer(GL_ARRAY_BUFFER, vbos_[VBOUVs]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * attribute_.uvs.size(), &attribute_.uvs[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(VBOUVs);
	glVertexAttribPointer(VBOUVs, 2, GL_FLOAT, GL_FALSE, 0, nullptr);

	glBindBuffer(GL_ARRAY_BUFFER, vbos_[VBONormals]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * attribute_.normals.size(), &attribute_.normals[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(VBONormals);
	glVertexAttribPointer(VBONormals, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbos_[VBOIndices]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned) * attribute_.indices.size(), &attribute_.indices[0], GL_STATIC_DRAW);
}

void SurfaceInternal::InitAttribute(const aiMesh* mesh) {
	const aiVector3D zero(0);
	for (unsigned i = 0; i < mesh->mNumVertices; ++i) {
		const aiVector3D* pos = &mesh->mVertices[i];
		const aiVector3D* normal = &mesh->mNormals[i];
		const aiVector3D* texCoord = mesh->HasTextureCoords(0) ? &(mesh->mTextureCoords[0][i]) : &zero;
		const aiVector3D* tangent = (mesh->mTangents != nullptr) ? &mesh->mTangents[i] : &zero;

		attribute_.positions.push_back(glm::vec3(pos->x, pos->y, pos->z));
		attribute_.normals.push_back(glm::vec3(normal->x, normal->y, normal->z));
		attribute_.uvs.push_back(glm::vec2(texCoord->x, texCoord->y));
	}

	for (unsigned i = 0; i < mesh->mNumFaces; ++i) {
		const aiFace& face = mesh->mFaces[i];
		AssertX(face.mNumIndices == 3, "invalid index count");
		attribute_.indices.push_back(face.mIndices[0]);
		attribute_.indices.push_back(face.mIndices[1]);
		attribute_.indices.push_back(face.mIndices[2]);
	}
}

bool SurfaceInternal::InitMaterials(const aiScene* scene, const std::string& path) {
	sceneMaterials_.resize(scene->mNumMaterials);

	std::string dir = Path::GetDirectory(path);

	bool status = true;
	for (unsigned i = 0; i < scene->mNumMaterials; ++i) {
		const aiMaterial* mat = scene->mMaterials[i];
		
		if (mat->GetTextureCount(aiTextureType_DIFFUSE) > 0) {
			aiString dpath;
			if (mat->GetTexture(aiTextureType_DIFFUSE, 0, &dpath) != AI_SUCCESS) {
				continue;
			}

			std::string fullPath = "textures/";
			fullPath += dpath.data;
			
			Texture2D* texture = Factory::Create<Texture2DInternal>();
			
			if (!texture->Load(fullPath)) {
				Factory::Release(texture);
				status = false;
				continue;
			}

			sceneMaterials_[i] = Factory::Create<MaterialInternal>();
			sceneMaterials_[i]->SetDiffuseTexture(texture);
		}
	}

	return status;
}

void SurfaceInternal::Bind() {
	glBindVertexArray(vao_);
}

void SurfaceInternal::Unbind() {
	glBindVertexArray(0);
}

#ifdef __UNUSED__
void SurfaceInternal::Render(GLenum mode) {
	glBindVertexArray(vao_);

	AssertX(mode == GL_TRIANGLES || mode == GL_PATCHES, "invalid mode");

	for (unsigned i = 0; i < meshes_.size(); ++i) {
		
		Texture* diffuse = meshes_[i].material->GetDiffuseTexture();
		
		if (diffuse) {
			diffuse->Bind(GL_TEXTURE0/*Globals::ColorTexture*/);
		}

		glDrawElementsBaseVertex(mode, meshes_[i].numIndices, GL_UNSIGNED_INT,
								 (void*)(sizeof(unsigned)* meshes_[i].baseIndex), meshes_[i].baseVertex);

	}

	glBindVertexArray(0);
}
#endif
