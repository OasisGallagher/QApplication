#include "animationinternal.h"

void AnimationClipInternal::SetDuration(float value) {
}

float AnimationClipInternal::GetDuration() {
	return 0.0f;
}

void AnimationClipInternal::SetCurve(AnimationCurve value) {
}

void AnimationClipInternal::Sample(float time, glm::mat4 & matrix) {
}

void AnimationKeysInternal::AddPosition(float time, const glm::vec3 & value) {
}

void AnimationKeysInternal::AddRotation(float time, const glm::quat & value) {
}

void AnimationKeysInternal::AddScale(float time, const glm::vec3 & value) {
}

void AnimationKeysInternal::RemovePosition(float time) {
}

void AnimationKeysInternal::RemoveRotation(float time) {
}

void AnimationKeysInternal::RemoveScale(float time) {
}

void AnimationKeysInternal::ToKeyframes(std::vector<AnimationKeyframe>& keyframes) {
}

void AnimationInternal::AddClip(const std::string & name, AnimationClip value) {
}

AnimationClip AnimationInternal::GetClip(const std::string & name) {
	return AnimationClip();
}

void AnimationInternal::SetRootTransform(const glm::mat4 & value) {
}

bool AnimationInternal::Play(const std::string & name) {
	return false;
}

void AnimationKeyframeInternal::Set(float time, const glm::vec3 & position, const glm::quat & rotation, const glm::vec3 & scale) {
}

void AnimationCurveInternal::SetKeyframes(const std::vector<AnimationKeyframe>& keyframes) {
}

void AnimationCurveInternal::Sample(float time, glm::vec3 & position, glm::quat & rotation, glm::vec3 & scale) {
}
