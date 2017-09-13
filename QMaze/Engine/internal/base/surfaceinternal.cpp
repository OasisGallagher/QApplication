#include <algorithm>
#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <glm/gtc/matrix_transform.hpp>

#include "tools/path.h"
#include "tools/debug.h"
#include "tools/mathf.h"
#include "variables.h"
#include "surfaceinternal.h"
#include "internal/memory/memory.h"
#include "internal/memory/factory.h"
#include "internal/base/meshinternal.h"
#include "internal/base/textureinternal.h"
#include "internal/base/materialinternal.h"
#include "internal/resources/resources.h"

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
	: ObjectInternal(ObjectTypeSurface), vao_(0) {
	std::fill(vbos_, vbos_ + CountOf(vbos_), 0);
}

SurfaceInternal::~SurfaceInternal() {
	Clear();
}

void SurfaceInternal::Clear() {
	glDeleteVertexArrays(1, &vao_);
	vao_ = 0;

	glDeleteVertexArrays(CountOf(vbos_), vbos_);
	std::fill(vbos_, vbos_ + CountOf(vbos_), 0);

	meshes_.clear();
}

void SurfaceInternal::SetAttribute(const SurfaceAttribute& value) {
	Clear();
	UpdateGLBuffers(value);
}

void SurfaceInternal::AddMesh(Mesh mesh) {
	meshes_.push_back(mesh);
}

Mesh SurfaceInternal::GetMesh(int index) {
	return meshes_[index];
}

int SurfaceInternal::GetMeshCount() const {
	return meshes_.size();
}

bool SurfaceInternal::Load(const std::string& path) {
	Clear();

	glGenVertexArrays(1, &vao_);
	Bind();

	glGenBuffers(CountOf(vbos_), vbos_);

	Assimp::Importer importer;
	unsigned flags = aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace;
	std::string fpath = "resources/" + path;
	const aiScene* scene = importer.ReadFile(fpath.c_str(), flags);

	AssertX(scene != nullptr, "failed to read file " + fpath + ": " + importer.GetErrorString());

	bool result = InitFromScene(scene, fpath);

	Unbind();

	return result;
}

bool SurfaceInternal::InitFromScene(const aiScene* scene, const std::string& path) {
	MeshTextures* textures = Memory::CreateArray<MeshTextures>(scene->mNumMaterials);
	InitTextures(scene, path, textures);

	InitMeshes(scene, textures);

	Memory::ReleaseArray(textures);

	return true;
}

void SurfaceInternal::InitMeshes(const aiScene* scene, MeshTextures* textures) {
	meshes_.reserve(scene->mNumMeshes);

	unsigned vertexCount = 0, indexCount = 0;

	for (unsigned i = 0; i < scene->mNumMeshes; ++i) {
		Mesh mesh = Factory::Create<MeshInternal>();
		mesh->SetTriangles(scene->mMeshes[i]->mNumFaces * 3, vertexCount, indexCount);

		if (scene->mMeshes[i]->mMaterialIndex < scene->mNumMaterials) {
			mesh->SetTextures(textures[scene->mMeshes[i]->mMaterialIndex]);
		}

		vertexCount += scene->mMeshes[i]->mNumVertices;
		indexCount += scene->mMeshes[i]->mNumFaces * 3;

		AddMesh(mesh);
	}

	InitMeshAttributes(scene, vertexCount, indexCount);
}

void SurfaceInternal::InitMeshAttributes(const aiScene* scene, unsigned numVertices, unsigned numIndices) {
	SurfaceAttribute attribute;
	attribute.positions.reserve(numVertices);
	attribute.normals.reserve(numVertices);
	attribute.uvs.reserve(numVertices);
	attribute.indices.reserve(numIndices);

	for (unsigned i = 0; i < scene->mNumMeshes; ++i) {
		InitAttribute(scene->mMeshes[i], attribute);
	}

	SetAttribute(attribute);
}

void SurfaceInternal::InitAttribute(const aiMesh* mesh, SurfaceAttribute& attribute) {
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

void SurfaceInternal::InitTextures(const aiScene* scene, const std::string& path, MeshTextures* textures) {
	std::string dir = Path::GetDirectory(path);

	for (unsigned i = 0; i < scene->mNumMaterials; ++i) {
		const aiMaterial* mat = scene->mMaterials[i];

		aiString dpath;
		std::string prefix = "textures/";
		if (mat->GetTextureCount(aiTextureType_NORMALS) > 0) {
			if (mat->GetTexture(aiTextureType_NORMALS, 0, &dpath) == AI_SUCCESS) {
				Texture2D texture = Factory::Create<Texture2DInternal>();
				if (texture->Load(prefix + dpath.data)) {
					textures[i].normal = texture;
				}
			}
		}

		if (mat->GetTextureCount(aiTextureType_DIFFUSE) > 0) {
			if (mat->GetTexture(aiTextureType_DIFFUSE, 0, &dpath) == AI_SUCCESS) {
				Texture2D texture = Factory::Create<Texture2DInternal>();
				if (texture->Load(prefix + dpath.data)) {
					textures[i].diffuse = texture;
				}
			}
		}

		if (mat->GetTextureCount(aiTextureType_SPECULAR) > 0) {
			if (mat->GetTexture(aiTextureType_SPECULAR, 0, &dpath) == AI_SUCCESS) {
				Texture2D texture = Factory::Create<Texture2DInternal>();
				if (texture->Load(prefix + dpath.data)) {
					textures[i].specular = texture;
				}
			}
		}
	}
}

void SurfaceInternal::UpdateGLBuffers(const SurfaceAttribute& attribute) {
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
		
		ITexture* diffuse = meshes_[i].material->GetDiffuseTexture();
		
		if (diffuse) {
			diffuse->Bind(GL_TEXTURE0/*Globals::ColorTexture*/);
		}

		glDrawElementsBaseVertex(mode, meshes_[i].numIndices, GL_UNSIGNED_INT,
								 (void*)(sizeof(unsigned)* meshes_[i].baseIndex), meshes_[i].baseVertex);

	}

	glBindVertexArray(0);
}
#endif
