#include <set>
#include <algorithm>
#include <glm/gtc/matrix_transform.hpp>

#include "tools/mathf.h"
#include "animationinternal.h"

bool SkeletonInternal::AddBone(const SkeletonBone& bone) {
	if (GetBone(bone.name) != nullptr) {
		return false;
	}

	Assert(current_ < GLSL_MAX_BONE_COUNT);
	bones_[current_] = bone;

	boneMap_.insert(std::make_pair(bone.name, current_));
	++current_;

	return true;
}

SkeletonBone* SkeletonInternal::GetBone(int index) {
	Assert(index >= 0 && index < current_);
	return bones_ + index;
}

SkeletonBone* SkeletonInternal::GetBone(const std::string& name) {
	std::map<std::string, int>::iterator pos = boneMap_.find(name);
	if (pos == boneMap_.end()) {
		return nullptr;
	}

	return bones_ + pos->second;
}

void SkeletonInternal::SetBoneToRootSpaceMatrix(int index, const glm::mat4& value) {
	Assert(index >= 0 && index < current_);
	boneToRootSpaceMatrices_[index] = value;
}

int SkeletonInternal::GetBoneIndex(const std::string& name) {
	std::map<std::string, int>::iterator pos = boneMap_.find(name);
	if (pos == boneMap_.end()) {
		return -1;
	}

	return pos->second;
}

SkeletonNode* SkeletonInternal::CreateNode(const std::string& name, const glm::mat4& matrix, AnimationCurve curve) {
	SkeletonNode* node = Memory::Create<SkeletonNode>();
	node->name = name;
	node->matrix = matrix;
	node->curve = curve;
	node->parent = nullptr;
	return node;
}

void SkeletonInternal::AddNode(SkeletonNode* parent, SkeletonNode* child) {
	if (parent == nullptr) {
		DestroyNodeHierarchy(root_);
		root_ = child;
	}
	else {
		child->parent = parent;
		parent->children.push_back(child);
	}
}

void SkeletonInternal::DestroyNodeHierarchy(SkeletonNode*& node) {
	if (node == nullptr) { return; }
	for (int i = 0; i < node->children.size(); ++i) {
		DestroyNodeHierarchy(node->children[i]);
		Memory::Release(node->children[i]);
	}

	Memory::Release(node);
	node = nullptr;
}

void AnimationClipInternal::Sample(float time) {
	Skeleton skeleton = GetAnimation()->GetSkeleton();
	SkeletonNode* root = skeleton->GetRootNode();
	SampleHierarchy(time, root, glm::mat4(1));
}

void AnimationClipInternal::SampleHierarchy(float time, SkeletonNode* node, const glm::mat4& matrix) {
	glm::mat4 transform = node->matrix;
	if (node->curve) {
		glm::quat rotation;
		glm::vec3 position, scale;
		node->curve->Sample(time, position, rotation, scale);
		glm::mat4 identity;
		transform = glm::translate(identity, position) * glm::mat4(rotation) * glm::scale(identity, scale);
	}

	transform = matrix * transform;

	Skeleton skeleton = GetAnimation()->GetSkeleton();
	int index = skeleton->GetBoneIndex(node->name);
	if (index >= 0) {
		skeleton->SetBoneToRootSpaceMatrix(index, transform);
	}

	for (int i = 0; i < node->children.size(); ++i) {
		SampleHierarchy(time, node->children[i], transform);
	}
}

void AnimationKeysInternal::ToKeyframes(std::vector<AnimationKeyframe>& keyframes) {
	std::set<float> times;
	for (int i = 0; i < scaleKeys_.size(); ++i) {
		times.insert(scaleKeys_[i].time);
	}

	for (int i = 0; i < rotationKeys_.size(); ++i) {
		times.insert(rotationKeys_[i].time);
	}

	for (int i = 0; i < positionKeys_.size(); ++i) {
		times.insert(positionKeys_[i].time);
	}

	for (std::set<float>::iterator ite = times.begin(); ite != times.end(); ++i) {
		float time = *ite;
		std::vector<ScaleKey> pos = std::lower_bound(scaleKeys_.begin(), scaleKeys_.end(), *ite, KeyComparer());
		if (pos != scaleKeys_.end() && Mathf::Approximately(pos->time, time)) {
			continue;
		}

		ScaleKey key;
		if (pos == scaleKeys_.end()) {
			key.time = time;
			key.value = scaleKeys_.back();
		}
		else {
			ScaleKey key;
			key.time = time;
			if (pos == scaleKeys_.begin()) {
				key.value = scaleKeys_.front();
			}
			else {
				std::vector<ScaleKey>::iterator prev = pos;
				--prev;

				key.value = Mathf::Lerp(prev->value, pos->value, time - prev->time / (pos->time - prev->time));
			}
		}

		scaleKeys_.insert(pos, key);
	}
}

void AnimationInternal::AddClip(const std::string& name, AnimationClip value) {
	std::vector<Key>::iterator ite = std::lower_bound(clips_.begin(), clips_.end(), name, KeyComparer());
	if(ite != clips_.end()){
		ite->value = value;
	}
	else {
		Key key{ name, value };
		clips_.insert(ite, key);
	}
}

AnimationClip AnimationInternal::GetClip(const std::string& name) {
	std::vector<Key>::iterator ite = std::lower_bound(clips_.begin(), clips_.end(), name, KeyComparer());
	if (ite == clips_.end()) {
		return nullptr;
	}

	return ite->value;
}

bool AnimationInternal::Play(const std::string& name) {
	return false;
}

void AnimationCurveInternal::Sample(float time, glm::vec3& position, glm::quat& rotation, glm::vec3& scale) {
	int index = FindInterpolateIndex(time);
	if (index < 0) { return; }

	int next = index + 1;

	float deltaTime = keyframes_[next]->GetTime() - keyframes_[index]->GetTime();
	float factor = (time - keyframes_[index]->GetTime()) / deltaTime;

	Assert(factor >= 0 && factor <= 1);

	const int p = KeyframeAttributePosition, r = KeyframeAttributeRotation, s = KeyframeAttributeScale;

	position = Mathf::Lerp(keyframes_[index]->GetVector3(p), keyframes_[next]->GetVector3(p), factor);
	rotation = Mathf::Lerp(keyframes_[index]->GetQuaternion(r), keyframes_[next]->GetQuaternion(r), factor);
	scale = Mathf::Lerp(keyframes_[index]->GetVector3(s), keyframes_[next]->GetVector3(s), factor);
}

int AnimationCurveInternal::FindInterpolateIndex(float time) {
	for (int i = 0; i < keyframes_.size(); ++i) {
		if (time < keyframes_[i + 1]->GetTime()) {
			return i;
		}
	}

	return -1;
}

void AnimationKeyframeInternal::SetVector3(int id, const glm::vec3& value) {
	std::vector<Key>::iterator ite = std::lower_bound(attributes_.begin(), attributes_.end(), id, KeyComparer());
	if (ite->id == id) {
		ite->value.SetVector3(value);
	}
	else {
		Key key = { id };
		key.value.SetVector3(value);
		attributes_.insert(ite, key);
	}
}

void AnimationKeyframeInternal::SetQuaternion(int id, const glm::quat& value) {
	std::vector<Key>::iterator ite = std::lower_bound(attributes_.begin(), attributes_.end(), id, KeyComparer());
	if (ite->id == id) {
		ite->value.SetQuaternion(value);
	}
	else {
		Key key = { id };
		key.value.SetQuaternion(value);
		attributes_.insert(ite, key);
	}
}

glm::vec3 AnimationKeyframeInternal::GetVector3(int id) {
	std::vector<Key>::iterator ite = std::lower_bound(attributes_.begin(), attributes_.end(), id, KeyComparer());
	AssertX(ite->id == id, "Animation keyframe attribute for id " + std::to_string(id) + " does not exist");
	return ite->value.GetVector3();
}

glm::quat AnimationKeyframeInternal::GetQuaternion(int id) {
	std::vector<Key>::iterator ite = std::lower_bound(attributes_.begin(), attributes_.end(), id, KeyComparer());
	AssertX(ite->id == id, "Animation keyframe attribute for id " + std::to_string(id) + " does not exist");
	return ite->value.GetQuaternion();
}
