#include <map>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include "variables.h"
#include "animation.h"
#include "glsldefines.h"
#include "internal/containers/variant.h"
#include "internal/base/objectinternal.h"

class SkeletonInternal : public ISkeleton, public ObjectInternal {
	DEFINE_FACTORY_METHOD(Skeleton)

public:
	SkeletonInternal() : ObjectInternal(ObjectTypeSkeleton), current_(0), root_(nullptr) {}
	~SkeletonInternal() { DestroyNodeHierarchy(root_); }

public:
	virtual bool AddBone(const SkeletonBone& bone);
	virtual SkeletonBone* GetBone(int index);
	virtual SkeletonBone* GetBone(const std::string& name);
	
	virtual void SetBoneToRootSpaceMatrix(int index, const glm::mat4& value);
	virtual glm::mat4* GetBoneToRootSpaceMatrices() { return boneToRootSpaceMatrices_; }

	virtual int GetBoneIndex(const std::string& name);
	virtual int GetBoneCount() { return current_; }

	virtual SkeletonNode* CreateNode(const std::string& name, const glm::mat4& matrix, AnimationCurve curve);
	virtual void AddNode(SkeletonNode* parent, SkeletonNode* child);
	virtual SkeletonNode* GetRootNode() { return root_; }

private:
	void DestroyNodeHierarchy(SkeletonNode*& node);

private:
	int current_;
	SkeletonBone bones_[C_MAX_BONE_COUNT];
	glm::mat4 boneToRootSpaceMatrices_[C_MAX_BONE_COUNT];

	SkeletonNode* root_;

	std::map<std::string, int> boneMap_;
};

class AnimationClipInternal : public IAnimationClip, public ObjectInternal {
	DEFINE_FACTORY_METHOD(AnimationClip)

public:
	AnimationClipInternal() : ObjectInternal(ObjectTypeAnimationClip) {}

public:
	virtual void SetWrapMode(AnimationWrapMode value) { wrapMode_ = value; }
	virtual AnimationWrapMode GetWrapMode() { return wrapMode_; }

	virtual void SetTicksPerSecond(float value);
	virtual float GetTicksPerSecond() { return ticksInSecond_; }

	virtual void SetDuration(float value) { duration_ = value; }
	virtual float GetDuration() { return duration_; }

	virtual void SetAnimation(Animation value) { animation_ = value; }
	virtual Animation GetAnimation() { return animation_.lock(); }

	virtual void Sample(float time);

private:
	void SampleHierarchy(float time, SkeletonNode* node, const glm::mat4& matrix);

private:
	float duration_;
	float ticksInSecond_;
	AnimationWrapMode wrapMode_;
	std::weak_ptr<Animation::element_type> animation_;
};

class AnimationStateInternal : public IAnimationState, public ObjectInternal {
	DEFINE_FACTORY_METHOD(AnimationState)

public:
	AnimationStateInternal() : ObjectInternal(ObjectTypeAnimationState) {}
};

class AnimationKeysInternal : public IAnimationKeys, public ObjectInternal {
	DEFINE_FACTORY_METHOD(AnimationKeys)

public:
	AnimationKeysInternal() :ObjectInternal(ObjectTypeAnimationKeys) {}

public:
	virtual void AddPosition(float time, const glm::vec3& value) { AddKey(positionKeys_, time, value); }
	virtual void AddRotation(float time, const glm::quat& value) { AddKey(rotationKeys_, time, value); }
	virtual void AddScale(float time, const glm::vec3& value) { AddKey(scaleKeys_, time, value); }

	virtual void RemovePosition(float time) { RemoveKey(positionKeys_, time); }
	virtual void RemoveRotation(float time) { RemoveKey(rotationKeys_, time); }
	virtual void RemoveScale(float time) { RemoveKey(scaleKeys_, time); }

	virtual void ToKeyframes(std::vector<AnimationKeyframe>& keyframes);

private:
	void SmoothKeys();

private:
	struct Key {
		float time;
	};

	struct KeyComparer {
		bool operator () (const Key& key, float time) const {
			return key.time < time;
		}
	};

	struct ScaleKey : Key {
		typedef glm::vec3 T;
		T value;
	};

	struct PositionKey : Key {
		typedef glm::vec3 T;
		T value;
	};

	struct RotationKey : Key {
		typedef glm::quat T;
		T value;
	};

private:
	template <class KeyType>
	void AddKey(std::vector<KeyType>& container, float time, const typename KeyType::T& value);

	template <class KeyType>
	void RemoveKey(std::vector<KeyType>& container, float time);

	template <class KeyType>
	void SmoothKey(std::vector<KeyType>& container, float time);

private:
	// TODO: sorted list.
	std::vector<ScaleKey> scaleKeys_;
	std::vector<RotationKey> rotationKeys_;
	std::vector<PositionKey> positionKeys_;
};

class AnimationInternal : public IAnimation, public ObjectInternal {
	DEFINE_FACTORY_METHOD(Animation)

public:
	AnimationInternal() : ObjectInternal(ObjectTypeAnimation), time_(0) {}

public:
	virtual void AddClip(const std::string& name, AnimationClip value);
	virtual AnimationClip GetClip(const std::string& name);

	virtual void SetRootTransform(const glm::mat4& value) { rootTransform_ = value; }
	virtual glm::mat4 GetRootTransform() { return rootTransform_; }

	virtual bool Play(const std::string& name);

	virtual void Update();

	virtual void SetSkeleton(Skeleton value) { skeleton_ = value; }
	virtual Skeleton GetSkeleton() { return skeleton_; }

	struct Key {
		std::string name;
		AnimationClip value;
	};

	struct KeyComparer {
		bool operator ()(const Key& key, const std::string& name) {
			return key.name < name;
		}
	};

private:
	Skeleton skeleton_;
	glm::mat4 rootTransform_;

	float time_;
	AnimationClip current_;

	// TODO: sorted list.
	std::vector<Key> clips_;
};

class AnimationKeyframeInternal : public IAnimationKeyframe, public ObjectInternal {
	DEFINE_FACTORY_METHOD(AnimationKeyframe)

public:
	AnimationKeyframeInternal() :ObjectInternal(ObjectTypeAnimationKeyframe) {}

public:
	virtual void SetTime(float value) { time_ = value; }
	virtual float GetTime() { return time_; }

	virtual void SetVector3(int id, const glm::vec3& value);
	virtual void SetQuaternion(int id, const glm::quat& value);
	virtual glm::vec3 GetVector3(int id);
	virtual glm::quat GetQuaternion(int id);

private:
	struct Key {
		int id;
		Variant value;
	};

	struct KeyComparer {
		bool operator () (const Key& key, int id) const {
			return key.id < id;
		}
	};

	float time_;
	// TODO: sorted list.
	std::vector<Key> attributes_;
};

class AnimationCurveInternal : public IAnimationCurve, public ObjectInternal {
	DEFINE_FACTORY_METHOD(AnimationCurve)
	
public:
	AnimationCurveInternal() :ObjectInternal(ObjectTypeAnimationCurve) {}

public:
	virtual void SetKeyframes(const std::vector<AnimationKeyframe>& value) { keyframes_ = value; }
	virtual void Sample(float time, glm::vec3& position, glm::quat& rotation, glm::vec3& scale);

private:
	int FindInterpolateIndex(float time);

private:
	std::vector<AnimationKeyframe> keyframes_;
};

#include "animationinternal.inl"
