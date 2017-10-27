#include <set>
#include <algorithm>
#include <glm/gtc/matrix_transform.hpp>

#include "tools/mathf.h"
#include "animationinternal.h"
#include "internal/misc/timefinternal.h"

#define DEFAULT_TICKS_PER_SECOND	25

bool SkeletonInternal::AddBone(const SkeletonBone& bone) {
	if (GetBone(bone.name) != nullptr) {
		return false;
	}

	Assert(current_ < C_MAX_BONE_COUNT);
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

AnimationClipInternal::AnimationClipInternal() : ObjectInternal(ObjectTypeAnimationClip), wrapper_(Mathf::Min) {
}

void AnimationClipInternal::SetWrapMode(AnimationWrapMode value) {
	switch (wrapMode_ = value) {
	case AnimationWrapModeLoop:
		wrapper_ = Mathf::Repeat;
		break;
	case AnimationWrapModePingPong:
		wrapper_ = Mathf::PingPong;
		break;
	case AnimationWrapModeOnce:
	case AnimationWrapModeClampForever:
		wrapper_ = Mathf::Min;
		break;
	}
}

void AnimationClipInternal::SetTicksPerSecond(float value) {
	if (Mathf::Approximately(value)) {
		value = DEFAULT_TICKS_PER_SECOND;
	}

	ticksInSecond_ = value;
}

bool AnimationClipInternal::Sample(float time) {
	time = wrapper_(time * GetTicksPerSecond(), GetDuration());

	Skeleton skeleton = GetAnimation()->GetSkeleton();
	SkeletonNode* root = skeleton->GetRootNode();

	return SampleHierarchy(time, root, glm::mat4(1));
}

bool AnimationClipInternal::SampleHierarchy(float time, SkeletonNode* node, const glm::mat4& matrix) {
	bool lastFrame = true;
	glm::mat4 transform = node->matrix;
	if (node->curve) {
		glm::quat rotation;
		glm::vec3 position, scale;
		lastFrame = node->curve->Sample(time, position, rotation, scale);
		glm::mat4 identity;
		transform = glm::translate(identity, position) * glm::mat4(rotation) * glm::scale(identity, scale);
	}

	transform = matrix * transform;

	Skeleton skeleton = GetAnimation()->GetSkeleton();
	int index = skeleton->GetBoneIndex(node->name);
	if (index >= 0) {
		glm::mat4 boneToRootSpaceMatrix = GetAnimation()->GetRootTransform();
		boneToRootSpaceMatrix *= transform * skeleton->GetBone(index)->localToBoneSpaceMatrix;

		skeleton->SetBoneToRootSpaceMatrix(index, boneToRootSpaceMatrix);
	}

	for (int i = 0; i < node->children.size(); ++i) {
		lastFrame = SampleHierarchy(time, node->children[i], transform) && lastFrame;
	}

	return lastFrame;
}

void AnimationKeysInternal::AddPosition(float time, const glm::vec3& value) {
	PositionKey key;
	key.time = time, key.value = value;
	positionKeys_.insert(key);
}

void AnimationKeysInternal::AddRotation(float time, const glm::quat& value) {
	RotationKey key;
	key.time = time, key.value = value;
	rotationKeys_.insert(key);
}

void AnimationKeysInternal::AddScale(float time, const glm::vec3 & value) {
	ScaleKey key;
	key.time = time, key.value = value;
	scaleKeys_.insert(key);
}

void AnimationKeysInternal::RemovePosition(float time) {
	PositionKey key;
	key.time = time;
	positionKeys_.remove(key);
}

void AnimationKeysInternal::RemoveRotation(float time) {
	RotationKey key;
	key.time = time;
	rotationKeys_.remove(key);
}

void AnimationKeysInternal::RemoveScale(float time) {
	ScaleKey key;
	key.time = time;
	scaleKeys_.remove(key);
}

void AnimationKeysInternal::ToKeyframes(std::vector<AnimationKeyframe>& keyframes) {
	SmoothKeys();

	Assert(positionKeys_.size() == rotationKeys_.size() && rotationKeys_.size() == scaleKeys_.size());
	keyframes.reserve(positionKeys_.size());

	for (int i = 0; i < positionKeys_.size(); ++i) {
		AnimationKeyframe keyframe = CREATE_OBJECT(AnimationKeyframe);
		keyframe->SetTime(positionKeys_[i].time);
		keyframe->SetVector3(KeyframeAttributePosition, positionKeys_[i].value);
		keyframe->SetQuaternion(KeyframeAttributeRotation, rotationKeys_[i].value);
		keyframe->SetVector3(KeyframeAttributeScale, scaleKeys_[i].value);

		keyframes.push_back(keyframe);
	}
}

void AnimationKeysInternal::SmoothKeys() {
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

	for (std::set<float>::iterator ite = times.begin(); ite != times.end(); ++ite) {
		float time = *ite;
		SmoothKey(positionKeys_, time);
		SmoothKey(rotationKeys_, time);
		SmoothKey(scaleKeys_, time);
	}
}

void AnimationInternal::AddClip(const std::string& name, AnimationClip value) {
	Key key{ name, value };
	clips_.insert(key);

	value->SetAnimation(dsp_cast<Animation>(shared_from_this()));
}

AnimationClip AnimationInternal::GetClip(const std::string& name) {
	Key key{ name };
	if (!clips_.get(key)) { return nullptr; }

	return key.value;
}

void AnimationInternal::SetWrapMode(AnimationWrapMode value) {
	for (int i = 0; i < clips_.size(); ++i) {
		clips_[i].value->SetWrapMode(value);
	}
}

bool AnimationInternal::Play(const std::string& name) {
	AnimationClip clip = GetClip(name);
	if (!clip) {
		Debug::LogWarning("can not find animation clip " + name);
		return false;
	}

	time_ = 0;
	current_ = clip;
	playing_ = true;

	return true;
}

void AnimationInternal::Update() {
	if (!playing_ || !current_) { return; }

	time_ += timeInstance->GetDeltaTime();
	
	if (current_->Sample(time_) && current_->GetWrapMode() == AnimationWrapModeOnce) {
		current_->Sample(0);
		playing_ = false;
	}
}

bool AnimationCurveInternal::Sample(float time, glm::vec3& position, glm::quat& rotation, glm::vec3& scale) {
	int index = FindInterpolateIndex(time);
	if (index + 1 >= keyframes_.size()) {
		SampleLastFrame(position, rotation, scale);
		return true;
	}

	Interpolate(index, time, position, rotation, scale);
	return false;
}

int AnimationCurveInternal::FindInterpolateIndex(float time) {
	int pos = 0;
	for (; pos < (int)keyframes_.size() - 1; ++pos) {
		if (time < keyframes_[pos + 1]->GetTime()) {
			break;
		}
	}

	return pos;
}

void AnimationCurveInternal::SampleLastFrame(glm::vec3& position, glm::quat& rotation, glm::vec3& scale) {
	if (!keyframes_.empty()) {
		AnimationKeyframe& frame = keyframes_.back();
		position = frame->GetVector3(KeyframeAttributePosition);
		rotation = frame->GetQuaternion(KeyframeAttributeRotation);
		scale = frame->GetVector3(KeyframeAttributeScale);
	}
}

void AnimationCurveInternal::Interpolate(int index, float time, glm::vec3& position, glm::quat& rotation, glm::vec3& scale) {
	int next = index + 1;

	float deltaTime = keyframes_[next]->GetTime() - keyframes_[index]->GetTime();
	float factor = (time - keyframes_[index]->GetTime()) / deltaTime;

	factor = Mathf::Clamp01(factor);

	const int p = KeyframeAttributePosition, r = KeyframeAttributeRotation, s = KeyframeAttributeScale;

	position = Mathf::Lerp(keyframes_[index]->GetVector3(p), keyframes_[next]->GetVector3(p), factor);
	rotation = Mathf::Lerp(keyframes_[index]->GetQuaternion(r), keyframes_[next]->GetQuaternion(r), factor);
	scale = Mathf::Lerp(keyframes_[index]->GetVector3(s), keyframes_[next]->GetVector3(s), factor);
}

void AnimationKeyframeInternal::SetVector3(int id, const glm::vec3& value) {
	Key key = { id };
	key.value.SetVector3(value);
	attributes_.insert(key);
}

void AnimationKeyframeInternal::SetQuaternion(int id, const glm::quat& value) {
	Key key = { id };
	key.value.SetQuaternion(value);
	attributes_.insert(key);
}

glm::vec3 AnimationKeyframeInternal::GetVector3(int id) {
	Key key{ id };
	if (!attributes_.get(key)) {
		Debug::LogError("Animation keyframe attribute for id " + std::to_string(id) + " does not exist");
		return glm::vec3(0);
	}

	return key.value.GetVector3();
}

glm::quat AnimationKeyframeInternal::GetQuaternion(int id) {
	Key key{ id };
	if (!attributes_.get(key)) {
		Debug::LogError("Animation keyframe attribute for id " + std::to_string(id) + " does not exist");
		return glm::quat();
	}

	return key.value.GetQuaternion();
}
