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
#include "internal/misc/idcreater.h"
#include "internal/base/meshinternal.h"
#include "internal/base/textureinternal.h"
#include "internal/base/materialinternal.h"

static void AIMaterixToGlm(glm::mat4& answer, const aiMatrix4x4& mat) {
	answer = glm::mat4(
		mat.a1, mat.b1, mat.c1, mat.d1,
		mat.a2, mat.b2, mat.c2, mat.d2,
		mat.a3, mat.b3, mat.c3, mat.d3,
		mat.a4, mat.b4, mat.c4, mat.d4
		);
}

static void AIMaterixToGlm(glm::mat4& answer, const aiMatrix3x3& mat) {
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

	glGenVertexArrays(1, &vao_);

	Bind();
	glGenBuffers(CountOf(vbos_), vbos_);
	Unbind();
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
	meshes_.clear();
	Bind();
	UpdateGLBuffers(value);
	Unbind();
}

void SurfaceInternal::AddMesh(Mesh mesh) {
	meshes_.push_back(mesh);
}

Mesh SurfaceInternal::GetMesh(int index) {
	return meshes_[index];
}

int SurfaceInternal::GetMeshCount() {
	return meshes_.size();
}

bool SurfaceInternal::Load(const std::string& path) {
	Bind();
	Assimp::Importer importer;
	unsigned flags = aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_CalcTangentSpace;
	std::string fpath = "resources/" + path;
	const aiScene* scene = importer.ReadFile(fpath.c_str(), flags);

	AssertX(scene != nullptr, "failed to read file " + fpath + ": " + importer.GetErrorString());

	bool result = InitFromScene(scene, fpath);

	Unbind();

	return result;
}

bool SurfaceInternal::InitFromScene(const aiScene* scene, const std::string& path) {
	MaterialTextures* textures = Memory::CreateArray<MaterialTextures>(scene->mNumMaterials);
	InitTextures(scene, path, textures);

	InitMeshes(scene, textures);

	Memory::ReleaseArray(textures);

	return true;
}

void SurfaceInternal::InitMeshes(const aiScene* scene, MaterialTextures* textures) {
	meshes_.reserve(scene->mNumMeshes);

	unsigned vertexCount = 0, indexCount = 0;

	for (unsigned i = 0; i < scene->mNumMeshes; ++i) {
		Mesh mesh = Factory::Create<MeshInternal>();
		mesh->SetTriangles(scene->mMeshes[i]->mNumFaces * 3, vertexCount, indexCount);

		if (scene->mMeshes[i]->mMaterialIndex < scene->mNumMaterials) {
			mesh->SetMaterialTextures(textures[scene->mMeshes[i]->mMaterialIndex]);
		}

		vertexCount += scene->mMeshes[i]->mNumVertices;
		indexCount += scene->mMeshes[i]->mNumFaces * 3;

		AddMesh(mesh);
	}

	InitMeshAttributes(scene, vertexCount, indexCount);
}

void SurfaceInternal::InitMeshAttributes(const aiScene* scene, unsigned numVertices, unsigned numIndexes) {
	SurfaceAttribute attribute;
	attribute.positions.reserve(numVertices);
	attribute.normals.reserve(numVertices);
	attribute.texCoords.reserve(numVertices);
	attribute.tangents.reserve(numVertices);
	attribute.indexes.reserve(numIndexes);

	attribute.bones.resize(numVertices);
	for (int i = 0; i < numVertices; ++i) {
		memset(&attribute.bones[i], 0, sizeof(BoneAttribute));
	}

	for (unsigned i = 0; i < scene->mNumMeshes; ++i) {
		InitAttribute(scene->mMeshes[i], i, attribute);
	}

	UpdateGLBuffers(attribute);
}

void SurfaceInternal::InitAttribute(const aiMesh* aimesh, int nm, SurfaceAttribute& attribute) {
	const aiVector3D zero(0);
	for (unsigned i = 0; i < aimesh->mNumVertices; ++i) {
		const aiVector3D* pos = &aimesh->mVertices[i];
		const aiVector3D* normal = &aimesh->mNormals[i];
		const aiVector3D* texCoord = aimesh->HasTextureCoords(0) ? &(aimesh->mTextureCoords[0][i]) : &zero;
		const aiVector3D* tangent = (aimesh->mTangents != nullptr) ? &aimesh->mTangents[i] : &zero;

		attribute.positions.push_back(glm::vec3(pos->x, pos->y, pos->z));
		attribute.normals.push_back(glm::vec3(normal->x, normal->y, normal->z));
		attribute.texCoords.push_back(glm::vec2(texCoord->x, texCoord->y));
		attribute.tangents.push_back(glm::vec3(tangent->x, tangent->y, tangent->z));
	}

	for (unsigned i = 0; i < aimesh->mNumFaces; ++i) {
		const aiFace& face = aimesh->mFaces[i];
		AssertX(face.mNumIndices == 3, "invalid index count");
		attribute.indexes.push_back(face.mIndices[0]);
		attribute.indexes.push_back(face.mIndices[1]);
		attribute.indexes.push_back(face.mIndices[2]);
	}

	InitBoneAttribute(aimesh, nm, attribute);
}

void SurfaceInternal::InitBoneAttribute(const aiMesh* aimesh, int nm, SurfaceAttribute &attribute) {
	for (unsigned i = 0; i < aimesh->mNumBones; ++i) {
		unsigned index = 0;
		std::string name(aimesh->mBones[i]->mName.data);

		if (map_.find(name) == map_.end()) {
			index = bones_.size();
			Bone bone;
			AIMaterixToGlm(bone.localToBoneSpaceMatrix, aimesh->mBones[i]->mOffsetMatrix);
			bones_.push_back(bone);
			map_[name] = index;
		}
		else {
			index = map_[name];
		}

		for (unsigned j = 0; j < aimesh->mBones[i]->mNumWeights; ++j) {
			// Since vertex IDs are relevant to a single mesh and we store all meshes
			// in a single vector we add the base vertex ID of the current aiMesh to 
			// vertex ID from the mWeights array to get the absolute vertex ID.
			unsigned vertexCount, baseVertex, baseIndex;
			meshes_[nm]->GetTriangles(vertexCount, baseVertex, baseIndex);
			unsigned vertexID = baseVertex + aimesh->mBones[i]->mWeights[j].mVertexId;
			float weight = aimesh->mBones[i]->mWeights[j].mWeight;
			for (int k = 0; k < MaxBoneCount; ++k) {
				if (Mathf::Approximately(attribute.bones[vertexID].weights[k])) {
					attribute.bones[vertexID].indexes[k] = index;
					attribute.bones[vertexID].weights[k] = weight;
				}
			}
		}
	}
}

void SurfaceInternal::InitTextures(const aiScene* scene, const std::string& path, MaterialTextures* textures) {
	std::string dir = Path::GetDirectory(path);

	for (unsigned i = 0; i < scene->mNumMaterials; ++i) {
		const aiMaterial* mat = scene->mMaterials[i];

		aiString dpath;
		std::string prefix = "textures/";
		if (mat->GetTextureCount(aiTextureType_NORMALS) > 0) {
			if (mat->GetTexture(aiTextureType_NORMALS, 0, &dpath) == AI_SUCCESS) {
				Texture2D texture = Factory::Create<Texture2DInternal>();
				if (texture->Load(prefix + dpath.data)) {
					textures[i].bump = texture;
				}
			}
		}

		if (mat->GetTextureCount(aiTextureType_DIFFUSE) > 0) {
			if (mat->GetTexture(aiTextureType_DIFFUSE, 0, &dpath) == AI_SUCCESS) {
				Texture2D texture = Factory::Create<Texture2DInternal>();
				if (texture->Load(prefix + dpath.data)) {
					textures[i].albedo = texture;
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
	if (!attribute.positions.empty()) {
		glBindBuffer(GL_ARRAY_BUFFER, vbos_[VBOPositions]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * attribute.positions.size(), &attribute.positions[0], GL_STATIC_DRAW);
		glEnableVertexAttribArray(IndexPosition);
		glVertexAttribPointer(IndexPosition, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
	}

	if (!attribute.texCoords.empty()) {
		glBindBuffer(GL_ARRAY_BUFFER, vbos_[VBOTexCoords]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * attribute.texCoords.size(), &attribute.texCoords[0], GL_STATIC_DRAW);
		glEnableVertexAttribArray(IndexTexCoord);
		glVertexAttribPointer(IndexTexCoord, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
	}

	if (!attribute.normals.empty()) {
		glBindBuffer(GL_ARRAY_BUFFER, vbos_[VBONormals]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * attribute.normals.size(), &attribute.normals[0], GL_STATIC_DRAW);
		glEnableVertexAttribArray(IndexNormal);
		glVertexAttribPointer(IndexNormal, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
	}

	if (!attribute.tangents.empty()) {
		glBindBuffer(GL_ARRAY_BUFFER, vbos_[VBOTangents]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * attribute.tangents.size(), &attribute.tangents[0], GL_STATIC_DRAW);
		glEnableVertexAttribArray(IndexTangent);
		glVertexAttribPointer(IndexTangent, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
	}

	if (!attribute.bones.empty()) {
		glBindBuffer(GL_ARRAY_BUFFER, vbos_[VBOBones]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(Bone)* attribute.bones.size(), &attribute.bones[0], GL_STATIC_DRAW);

		glEnableVertexAttribArray(IndexBoneIndexes);
		glVertexAttribIPointer(IndexBoneIndexes, 4, GL_INT, sizeof(Bone), nullptr);

		glEnableVertexAttribArray(IndexBoneWeights);
		glVertexAttribPointer(IndexBoneWeights, 4, GL_FLOAT, GL_FALSE, sizeof(Bone), (const GLvoid*)(sizeof(unsigned) * MaxBoneCount));
	}

	if (!attribute.indexes.empty()) {
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbos_[VBOIndexes]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned) * attribute.indexes.size(), &attribute.indexes[0], GL_STATIC_DRAW);
	}
}

void SurfaceInternal::Bind() {
	glBindVertexArray(vao_);
}

void SurfaceInternal::Unbind() {
	glBindVertexArray(0);
}
