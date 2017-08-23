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

void SkinnedMesh::VertexBoneData::AddBoneData(unsigned boneID, float weight) {
	unsigned pos = 0;
	for (; pos < COUNT_OF(ids); ++pos) {
		if (weights[pos] == 0) {
			break;
		}
	}

	Assert(pos < COUNT_OF(ids), "too many bone data");
	ids[pos] = boneID;
	weights[pos] = weight;
}

SkinnedMesh::SkinnedMesh() {
	vao_ = 0;
	memset(vbos_, 0, sizeof(vbos_));

	boneCount_ = 0;
	scene_ = nullptr;
}

SkinnedMesh::~SkinnedMesh() {
	Clear();
}

void SkinnedMesh::Clear() {
	for (unsigned i = 0; i < textures_.size(); ++i) {
		delete textures_[i];
	}
	textures_.clear();

	glDeleteVertexArrays(1, &vao_);
	vao_ = 0;

	glDeleteBuffers(COUNT_OF(vbos_), vbos_);
	memset(vbos_, 0, sizeof(vbos_));
}

bool SkinnedMesh::Load(const std::string& path) {
	Clear();

	glGenVertexArrays(1, &vao_);
	glBindVertexArray(vao_);

	glGenBuffers(COUNT_OF(vbos_), vbos_);

	scene_ = importer_.ReadFile(path.c_str(), aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_JoinIdenticalVertices);

	Assert(scene_ != nullptr, "failed to load scene " + path);

	aiMatrix4x4 mat = scene_->mRootNode->mTransformation;
	mat.Inverse();
	aiMaterixToGlm(globalInverseTransform_, mat);

	bool success = InitFromScene(scene_, path);
	glBindVertexArray(0);

	return success;
}

bool SkinnedMesh::InitFromScene(const aiScene* scene, const std::string& path) {
	entries_.resize(scene_->mNumMeshes);
	textures_.resize(scene_->mNumMaterials);

	SkinnedMeshData data;

	unsigned numVertices = 0, numIndices = 0;

	for (unsigned i = 0; i < entries_.size(); ++i) {
		entries_[i].materialIndex = scene_->mMeshes[i]->mMaterialIndex;
		entries_[i].numIndices = scene_->mMeshes[i]->mNumFaces * 3;
		entries_[i].baseVertex = numVertices;
		entries_[i].baseIndex = numIndices;

		numVertices += scene_->mMeshes[i]->mNumVertices;
		numIndices += entries_[i].numIndices;
	}

	data.positions.reserve(numVertices);
	data.uvs.reserve(numVertices);
	data.normals.reserve(numVertices);
	data.bones.resize(numVertices);
	data.indices.reserve(numIndices);

	for (unsigned i = 0; i < entries_.size(); ++i) {
		InitMesh(i, scene->mMeshes[i], data);
	}

	if (!InitMaterials(scene, path)) {
		return false;
	}

	glBindBuffer(GL_ARRAY_BUFFER, vbos_[VBOPositions]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * data.positions.size(), &data.positions[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(LocationPosition);
	glVertexAttribPointer(LocationPosition, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

	glBindBuffer(GL_ARRAY_BUFFER, vbos_[VBOUVs]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * data.uvs.size(), &data.uvs[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(LocationUV);
	glVertexAttribPointer(LocationUV, 2, GL_FLOAT, GL_FALSE, 0, nullptr);

	glBindBuffer(GL_ARRAY_BUFFER, vbos_[VBONormals]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * data.normals.size(), &data.normals[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(LocationNormal);
	glVertexAttribPointer(LocationNormal, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

	glBindBuffer(GL_ARRAY_BUFFER, vbos_[VBOBones]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(VertexBoneData)* data.bones.size(), &data.bones[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(LocationBoneID);
	// It is very important to note that we use glVertexAttribIPointer rather than
	// glVertexAttribPointer to bind the IDs.The reason is that the IDs are integer 
	// and not floating point.Pay attention to this or you will get corrupted data 
	// in the shader.
	glVertexAttribIPointer(LocationBoneID, 4, GL_INT, sizeof(VertexBoneData), nullptr);

	glEnableVertexAttribArray(LocationBoneWeight);
	glVertexAttribPointer(LocationBoneWeight, 4, GL_FLOAT, GL_FALSE, sizeof(VertexBoneData), (const GLvoid*)16);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbos_[VBOIndices]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned)* data.indices.size(), &data.indices[0], GL_STATIC_DRAW);

	return true;
}

void SkinnedMesh::InitMesh(unsigned index, const aiMesh* mesh, SkinnedMeshData& data) {
	const aiVector3D zero(0);

	for (unsigned i = 0; i < mesh->mNumVertices; ++i) {
		const aiVector3D* pos = &mesh->mVertices[i];
		const aiVector3D* normal = &mesh->mNormals[i];
		const aiVector3D* uv = mesh->HasTextureCoords(0) ? &mesh->mTextureCoords[0][i] : &zero;

		data.positions.push_back(glm::vec3(pos->x, pos->y, pos->z));
		data.normals.push_back(glm::vec3(normal->x, normal->y, normal->z));
		data.uvs.push_back(glm::vec2(uv->x, uv->y));
	}

	LoadBones(index, mesh, data.bones);

	for (unsigned i = 0; i < mesh->mNumFaces; ++i) {
		const aiFace& face = mesh->mFaces[i];
		data.indices.push_back(face.mIndices[0]);
		data.indices.push_back(face.mIndices[1]);
		data.indices.push_back(face.mIndices[2]);
	}
}

void SkinnedMesh::LoadBones(unsigned meshIndex, const aiMesh* mesh, std::vector<VertexBoneData>& bones) {
	for (unsigned i = 0; i < mesh->mNumBones; ++i) {
		unsigned boneIndex = 0;
		std::string boneName(mesh->mBones[i]->mName.data);

		if (boneMap_.find(boneName) == boneMap_.end()) {
			boneIndex = boneCount_++;
			BoneInfo bi;
			aiMaterixToGlm(bi.boneOffset, mesh->mBones[i]->mOffsetMatrix);
			boneInfos_.push_back(bi);
			boneMap_[boneName] = boneIndex;
		}
		else {
			boneIndex = boneMap_[boneName];
		}

		for (unsigned j = 0; j < mesh->mBones[i]->mNumWeights; ++j) {
			// Since vertex IDs are relevant to a single mesh and we store all meshes
			// in a single vector we add the base vertex ID of the current aiMesh to 
			// vertex ID from the mWeights array to get the absolute vertex ID.
			unsigned vertexID = entries_[meshIndex].baseVertex + mesh->mBones[i]->mWeights[j].mVertexId;
			float weight = mesh->mBones[i]->mWeights[j].mWeight;
			bones[vertexID].AddBoneData(boneIndex, weight);
		}
	}
}

bool SkinnedMesh::InitMaterials(const aiScene* scene, const std::string& path) {
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

void SkinnedMesh::Render() {
	glBindVertexArray(vao_);

	for (unsigned i = 0; i < entries_.size(); ++i) {
		unsigned materilIndex = entries_[i].materialIndex;
		Assert(materilIndex < textures_.size(), "invalid materialIndex");

		if (textures_[materilIndex] != nullptr) {
			textures_[materilIndex]->Bind(Globals::ColorTexture);
		}

		glDrawElementsBaseVertex(GL_TRIANGLES, entries_[i].numIndices, GL_UNSIGNED_INT,
								 (void*)(sizeof(unsigned)* entries_[i].baseIndex), entries_[i].baseVertex);

	}

	glBindVertexArray(0);
}

unsigned SkinnedMesh::FindPosition(float time, const aiNodeAnim* node) {
	for (unsigned i = 0; i < node->mNumPositionKeys - 1; ++i) {
		if (time < (float)node->mPositionKeys[i + 1].mTime) {
			return i;
		}
	}

	Assert(false, "invalid time");
	return 0;
}

unsigned SkinnedMesh::FindRotation(float time, const aiNodeAnim* node) {
	for (unsigned i = 0; i < node->mNumRotationKeys - 1; ++i) {
		if (time < (float)node->mRotationKeys[i + 1].mTime) {
			return i;
		}
	}

	Assert(false, "invalid time");
	return 0;
}

unsigned SkinnedMesh::FindScaling(float time, const aiNodeAnim* node) {
	for (unsigned i = 0; i < node->mNumScalingKeys - 1; ++i) {
		if (time < (float)node->mScalingKeys[i + 1].mTime) {
			return i;
		}
	}

	Assert(false, "invalid time");
	return 0;
}

void SkinnedMesh::CalculateInterpolatedPosition(aiVector3D& answer, float time, const aiNodeAnim* node) {
	if (node->mNumPositionKeys == 1) {
		answer = node->mPositionKeys[0].mValue;
		return;
	}

	unsigned index = FindPosition(time, node);
	unsigned next = index + 1;

	float deltaTime = (float)(node->mPositionKeys[next].mTime - node->mPositionKeys[index].mTime);
	float factor = (float)(time - node->mPositionKeys[index].mTime) / deltaTime;
	Assert(factor >= 0 && factor <= 1, "invalid factor");

	const aiVector3D& from = node->mPositionKeys[index].mValue;
	const aiVector3D& to = node->mPositionKeys[next].mValue;
	answer = from + (to - from) * factor;
}

void SkinnedMesh::CalculateInterpolatedRotation(aiQuaternion& answer, float time, const aiNodeAnim* node) {
	if (node->mNumRotationKeys == 1) {
		answer = node->mRotationKeys[0].mValue;
		return;
	}

	unsigned index = FindRotation(time, node);
	unsigned next = index + 1;

	float deltaTime = (float)(node->mRotationKeys[next].mTime - node->mRotationKeys[index].mTime);
	float factor = (float)(time - node->mRotationKeys[index].mTime) / deltaTime;
	Assert(factor >= 0 && factor <= 1, "invalid factor");

	const aiQuaternion& from = node->mRotationKeys[index].mValue;
	const aiQuaternion& to = node->mRotationKeys[next].mValue;
	aiQuaternion::Interpolate(answer, from, to, factor);
	answer.Normalize();
}

void SkinnedMesh::CalculateInterpolatedScaling(aiVector3D& answer, float time, const aiNodeAnim* node) {
	if (node->mNumScalingKeys == 1) {
		answer = node->mScalingKeys[0].mValue;
		return;
	}

	unsigned index = FindScaling(time, node);
	unsigned next = index + 1;

	float deltaTime = (float)(node->mScalingKeys[next].mTime - node->mScalingKeys[index].mTime);
	float factor = (float)(time - node->mScalingKeys[index].mTime) / deltaTime;
	Assert(factor >= 0 && factor <= 1, "invalid factor");

	const aiVector3D& from = node->mScalingKeys[index].mValue;
	const aiVector3D& to = node->mScalingKeys[next].mValue;
	answer = from + (to - from) * factor;
}

void SkinnedMesh::ReadNodeHeirarchy(float time, const aiNode* node, const glm::mat4& parentTransform) {
	std::string nodeName(node->mName.data);
	// A single aiAnimation object represents a sequence of animation frames such 
	// as "walk", "run", "shoot", etc.
	const aiAnimation* anim = scene_->mAnimations[0];

	glm::mat4 nodeTransform;
	aiMaterixToGlm(nodeTransform, node->mTransformation);

	// The animation has an array of aiNodeAnim objects called channels.
	// Each channel is actually the bone with all its transformations. 
	// The channel contains a name which must match one of the nodes in the heirarchy
	// and three transformation arrays.
	const aiNodeAnim* nodeAnim = FindNodeAnim(anim, nodeName);

	if (nodeAnim != nullptr) {
		aiVector3D scaling;
		CalculateInterpolatedScaling(scaling, time, nodeAnim);
		glm::mat4 indentity;
		glm::mat4 scalingMatrix = glm::scale(indentity, glm::vec3(scaling.x, scaling.y, scaling.z));

		aiQuaternion rotation;
		CalculateInterpolatedRotation(rotation, time, nodeAnim);
		glm::mat4 rotationMatrix;
		aiMaterixToGlm(rotationMatrix, rotation.GetMatrix());

		aiVector3D translation;
		CalculateInterpolatedPosition(translation, time, nodeAnim);
		glm::mat4 translationMatrix = glm::translate(indentity, glm::vec3(translation.x, translation.y, translation.z));

		nodeTransform = translationMatrix * rotationMatrix * scalingMatrix;
	}

	glm::mat4 current = parentTransform * nodeTransform;

	if (boneMap_.find(nodeName) != boneMap_.end()) {
		unsigned boneIndex = boneMap_[nodeName];
		boneInfos_[boneIndex].finalTransform = globalInverseTransform_ * current * boneInfos_[boneIndex].boneOffset;
	}

	for (unsigned i = 0; i < node->mNumChildren; ++i) {
		ReadNodeHeirarchy(time, node->mChildren[i], current);
	}
}

void SkinnedMesh::GetBoneTransform(float time, std::vector<glm::mat4>& transforms) {
	float ticksPerSeconds = (float)scene_->mAnimations[0]->mTicksPerSecond;
	if (Utility::Approximately(ticksPerSeconds)) {
		ticksPerSeconds = 25.f;
	}

	float timeInTicks = time * ticksPerSeconds;
	float animationTime = fmod(timeInTicks, (float)scene_->mAnimations[0]->mDuration);

	ReadNodeHeirarchy(animationTime, scene_->mRootNode, glm::mat4(1));

	transforms.resize(boneCount_);
	for (unsigned i = 0; i < boneCount_; ++i) {
		transforms[i] = boneInfos_[i].finalTransform;
	}
}

const aiNodeAnim* SkinnedMesh::FindNodeAnim(const aiAnimation* anim, const std::string& name) {
	for (unsigned i = 0; i < anim->mNumChannels; ++i) {
		const aiNodeAnim* nodeAnim = anim->mChannels[i];

		if (name == nodeAnim->mNodeName.data) {
			return nodeAnim;
		}
	}

	return nullptr;
}
