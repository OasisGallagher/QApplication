#pragma once
#include <vector>
#include "object.h"

class IAnimation;
class IAnimationClip;
class IAnimationKeys;
class IAnimationCurve;
class IAnimationState;
class IAnimationKeyframe;

typedef std::shared_ptr<IAnimation> Animation;
typedef std::shared_ptr<IAnimationClip> AnimationClip;
typedef std::shared_ptr<IAnimationKeys> AnimationKeys;
typedef std::shared_ptr<IAnimationCurve> AnimationCurve;
typedef std::shared_ptr<IAnimationState> AnimationState;
typedef std::shared_ptr<IAnimationKeyframe> AnimationKeyframe;

struct Bone {
	glm::mat4 localToBoneSpaceMatrix;
	glm::mat4 boneToRootSpaceMatrix;
};

struct Skeleton {
	enum {
		MaxBoneCount = 128
	};

	int boneCount;
	Bone bones[MaxBoneCount];
};

class ENGINE_EXPORT IAnimationClip : virtual public IObject {
public:
	virtual void SetDuration(float value) = 0;
	virtual float GetDuration() = 0;
	virtual void SetCurve(AnimationCurve value) = 0;
	virtual void Sample(float time, glm::mat4& matrix) = 0;
};

class ENGINE_EXPORT IAnimationState : virtual public IObject {
};

class ENGINE_EXPORT IAnimationKeys : virtual public IObject {
public:
	virtual void AddPosition(float time, const glm::vec3& value) = 0;
	virtual void AddRotation(float time, const glm::quat& value) = 0;
	virtual void AddScale(float time, const glm::vec3& value) = 0;

	virtual void RemovePosition(float time) = 0;
	virtual void RemoveRotation(float time) = 0;
	virtual void RemoveScale(float time) = 0;

	virtual void ToKeyframes(std::vector<AnimationKeyframe>& keyframes) = 0;
};

class ENGINE_EXPORT IAnimationKeyframe : virtual public IObject {
public:
	virtual void Set(float time, const glm::vec3& position, const glm::quat& rotation, const glm::vec3& scale) = 0;
};

class ENGINE_EXPORT IAnimationCurve : virtual public IObject {
public:
	virtual void SetKeyframes(const std::vector<AnimationKeyframe>& keyframes) = 0;
	virtual void Sample(float time, glm::vec3& position, glm::quat& rotation, glm::vec3& scale) = 0;
};

class ENGINE_EXPORT IAnimation : virtual public IObject {
public:
	virtual void AddClip(const std::string& name, AnimationClip value) = 0;
	virtual AnimationClip GetClip(const std::string& name) = 0;

	virtual void SetRootTransform(const glm::mat4& value) = 0;

	virtual bool Play(const std::string& name) = 0;
};
