#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>

#include "tools/path.h"
#include "tools/debug.h"
#include "tools/mathf.h"
#include "modelimporter.h"
#include "internal/memory/memory.h"
#include "internal/memory/factory.h"
#include "internal/base/meshinternal.h"
#include "internal/base/shaderinternal.h"
#include "internal/base/textureinternal.h"
#include "internal/base/surfaceinternal.h"
#include "internal/base/materialinternal.h"
#include "internal/base/rendererinternal.h"
#include "internal/base/animationinternal.h"

static glm::mat4& AIMaterixToGLM(glm::mat4& answer, const aiMatrix4x4& mat) {
	answer = glm::mat4(
		mat.a1, mat.b1, mat.c1, mat.d1,
		mat.a2, mat.b2, mat.c2, mat.d2,
		mat.a3, mat.b3, mat.c3, mat.d3,
		mat.a4, mat.b4, mat.c4, mat.d4
	);

	return answer;
}

static glm::quat& AIQuaternionToGLM(glm::quat& answer, const aiQuaternion& quaternion) {
	answer = glm::quat(quaternion.w, quaternion.x, quaternion.y, quaternion.z);
	return answer;
}

static glm::vec3& AIVector3ToGLM(glm::vec3& answer, const aiVector3D& vec) {
	answer = glm::vec3(vec.x, vec.y, vec.z);
	return answer;
}

bool ModelImporter::Import(const std::string& path, int mask) {
	Assimp::Importer importer;
	unsigned flags = aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_CalcTangentSpace;
	std::string fpath = "resources/" + path;
	const aiScene* scene = importer.ReadFile(fpath.c_str(), flags);

	AssertX(scene != nullptr, "failed to read file " + fpath + ": " + importer.GetErrorString());
	Clear();

	path_ = fpath;
	scene_ = scene;

	if (!ImportSurface(surface_)) {
		return false;
	}

	if ((mask & MaskImportAnimation) != 0 && !ImportAnimation(animation_)) {
		return false;
	}

	if ((mask & MaskCreateRenderer) != 0) {
		return InitRenderer(animation_, renderer_);
	}

	return true;
}

void ModelImporter::Clear() {
	skeleton_.boneCount = 0;
	path_.clear();
	scene_ = nullptr;
	boneMap_.clear();
	surface_.reset();
	renderer_.reset();
	animation_.reset();
}

bool ModelImporter::ImportSurface(Surface& surface) {
	surface = Factory::Create<SurfaceInternal>();

	MaterialTextures* textures = Memory::CreateArray<MaterialTextures>(scene_->mNumMaterials);
	ImportTextures(textures);

	SurfaceAttribute attribute;
	ImportSurfaceAttributes(surface, attribute, textures);
	surface->SetAttribute(attribute);

	Memory::ReleaseArray(textures);
	return true;
}

void ModelImporter::ImportTextures(MaterialTextures* textures) {
	std::string dir = Path::GetDirectory(path_);

	for (unsigned i = 0; i < scene_->mNumMaterials; ++i) {
		const aiMaterial* mat = scene_->mMaterials[i];

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

void ModelImporter::ImportSurfaceAttributes(Surface surface, SurfaceAttribute& attribute, MaterialTextures* textures) {
	unsigned vertexCount = 0, indexCount = 0;

	for (unsigned i = 0; i < scene_->mNumMeshes; ++i) {
		Mesh mesh = Factory::Create<MeshInternal>();
		mesh->SetTriangles(scene_->mMeshes[i]->mNumFaces * 3, vertexCount, indexCount);

		if (scene_->mMeshes[i]->mMaterialIndex < scene_->mNumMaterials) {
			mesh->SetMaterialTextures(textures[scene_->mMeshes[i]->mMaterialIndex]);
		}

		vertexCount += scene_->mMeshes[i]->mNumVertices;
		indexCount += scene_->mMeshes[i]->mNumFaces * 3;

		surface->AddMesh(mesh);
	}

	attribute.positions.reserve(vertexCount);
	attribute.normals.reserve(vertexCount);
	attribute.texCoords.reserve(vertexCount);
	attribute.tangents.reserve(vertexCount);
	attribute.indexes.reserve(indexCount);
	attribute.blendAttrs.resize(vertexCount);

	for (int i = 0; i < vertexCount; ++i) {
		memset(&attribute.blendAttrs[i], 0, sizeof(BlendAttribute));
	}

	for (unsigned i = 0; i < scene_->mNumMeshes; ++i) {
		ImportMeshAttributes(scene_->mMeshes[i], i, attribute);
		ImportBoneAttributes(scene_->mMeshes[i], i, surface, attribute);
	}
}

void ModelImporter::ImportMeshAttributes(const aiMesh* aimesh, int nm, SurfaceAttribute& attribute) {
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
}

void ModelImporter::ImportBoneAttributes(const aiMesh* aimesh, int nm, Surface surface, SurfaceAttribute& attribute) {
	for (unsigned i = 0; i < aimesh->mNumBones; ++i) {
		unsigned index = 0;
		std::string name(aimesh->mBones[i]->mName.data);

		if (boneMap_.find(name) == boneMap_.end()) {
			Bone bone;
			AIMaterixToGLM(bone.localToBoneSpaceMatrix, aimesh->mBones[i]->mOffsetMatrix);
			skeleton_.bones[skeleton_.boneCount] = bone;
			boneMap_[name] = skeleton_.boneCount;
			++skeleton_.boneCount;
		}
		else {
			index = boneMap_[name];
		}

		for (unsigned j = 0; j < aimesh->mBones[i]->mNumWeights; ++j) {
			// Since vertex IDs are relevant to a single mesh and we store all meshes
			// in a single vector we add the base vertex ID of the current aiMesh to 
			// vertex ID from the mWeights array to get the absolute vertex ID.
			unsigned vertexCount, baseVertex, baseIndex;
			surface->GetMesh(nm)->GetTriangles(vertexCount, baseVertex, baseIndex);
			unsigned vertexID = baseVertex + aimesh->mBones[i]->mWeights[j].mVertexId;
			float weight = aimesh->mBones[i]->mWeights[j].mWeight;
			for (int k = 0; k < BlendAttribute::Quality; ++k) {
				if (Mathf::Approximately(attribute.blendAttrs[vertexID].weights[k])) {
					attribute.blendAttrs[vertexID].indexes[k] = index;
					attribute.blendAttrs[vertexID].weights[k] = weight;
					break;
				}
			}
		}
	}
}

bool ModelImporter::ImportAnimation(Animation& animation) {
	if (scene_->mNumAnimations == 0) {
		return true;
	}
	
	animation = Factory::Create<AnimationInternal>();

	glm::mat4 rootTransform;
	animation->SetRootTransform(AIMaterixToGLM(rootTransform, scene_->mRootNode->mTransformation.Inverse()));

	for (int i = 0; i < scene_->mNumAnimations; ++i) {
		aiAnimation* anim = scene_->mAnimations[i];
		std::string name = anim->mName.C_Str();

		AnimationClip clip = Factory::Create<AnimationClipInternal>();
		ImportAnimationClip(anim, clip);
		animation->AddClip(name, clip);
	}

	return true;
}

void ModelImporter::ImportAnimationClip(const aiAnimation* anim, AnimationClip clip) {
	clip->SetDuration((float)anim->mDuration);
	const aiNode* root = scene_->mRootNode;
	const aiNodeAnim* channel = FindChannel(anim, root->mName.C_Str());

	AnimationKeys keys = Factory::Create<AnimationKeysInternal>();
	if (channel != nullptr) {
		for (int i = 0; i < channel->mNumPositionKeys; ++i) {
			const aiVectorKey& key = channel->mPositionKeys[i];
			glm::vec3 position;
			keys->AddPosition((float)key.mTime, AIVector3ToGLM(position, key.mValue));
		}

		for (int i = 0; i < channel->mNumRotationKeys; ++i) {
			const aiQuatKey& key = channel->mRotationKeys[i];
			glm::quat rotation;
			keys->AddRotation((float)key.mTime, AIQuaternionToGLM(rotation, key.mValue));
		}

		for (int i = 0; i < channel->mNumScalingKeys; ++i) {
			const aiVectorKey& key = channel->mScalingKeys[i];
			glm::vec3 scale;
			keys->AddScale((float)key.mTime, AIVector3ToGLM(scale, key.mValue));
		}
	}

	std::vector<AnimationKeyframe> keyframes;
	keys->ToKeyframes(keyframes);

	AnimationCurve curve = Factory::Create<AnimationCurveInternal>();
	curve->SetKeyframes(keyframes);

	clip->SetCurve(curve);
}

const aiNodeAnim* ModelImporter::FindChannel(const aiAnimation* anim, const char* name) {
	for (int i = 0; i < anim->mNumChannels; ++i) {
		if (strcmp(anim->mChannels[i]->mNodeName.C_Str(), name) == 0) {
			return anim->mChannels[i];
		}
	}

	return nullptr;
}

bool ModelImporter::InitRenderer(Animation animation, Renderer& renderer) {
	if (!animation) {
		renderer = Factory::Create<SurfaceRendererInternal>();
	}
	else {
		SkinnedSurfaceRenderer skinnedSurfaceRenderer;
		renderer = skinnedSurfaceRenderer = Factory::Create<SkinnedSurfaceRendererInternal>();
	}

	renderer->SetRenderState(Cull, Off);
	renderer->SetRenderState(DepthTest, LessEqual);

	Shader shader = Factory::Create<ShaderInternal>();
	shader->Load("buildin/shaders/texture");

	Material material = Factory::Create<MaterialInternal>();
	material->SetShader(shader);
	renderer->AddMaterial(material);

	return true;
}
