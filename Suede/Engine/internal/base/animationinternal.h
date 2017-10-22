#include <glm/glm.hpp>

#include "animation.h"
#include "internal/base/objectinternal.h"

class AnimationClipInternal : virtual public IAnimationClip, public ObjectInternal {
	DEFINE_FACTORY_METHOD(AnimationClip)

public:
	AnimationClipInternal() : ObjectInternal(ObjectTypeAnimationClip) {}

public:
	virtual void SetDuration(float value);
	virtual float GetDuration();
	virtual void SetCurve(AnimationCurve value);
	virtual void Sample(float time, glm::mat4 & matrix);
};

class AnimationStateInternal : virtual public IAnimationState, public ObjectInternal {
	DEFINE_FACTORY_METHOD(AnimationState)

public:
	AnimationStateInternal() : ObjectInternal(ObjectTypeAnimationState) {}
};

class AnimationKeysInternal : virtual public IAnimationKeys, public ObjectInternal {
	DEFINE_FACTORY_METHOD(AnimationKeys)

public:
	AnimationKeysInternal() :ObjectInternal(ObjectTypeAnimationKeys) {}

public:
	virtual void AddPosition(float time, const glm::vec3 & value);
	virtual void AddRotation(float time, const glm::quat & value);
	virtual void AddScale(float time, const glm::vec3 & value);
	virtual void RemovePosition(float time);
	virtual void RemoveRotation(float time);
	virtual void RemoveScale(float time);
	virtual void ToKeyframes(std::vector<AnimationKeyframe>& keyframes);
};

class AnimationInternal : virtual public IAnimation, public ObjectInternal {
	DEFINE_FACTORY_METHOD(Animation)

public:
	AnimationInternal() : ObjectInternal(ObjectTypeAnimation) {}

public:
	virtual void AddClip(const std::string & name, AnimationClip value);
	virtual AnimationClip GetClip(const std::string & name);
	virtual void SetRootTransform(const glm::mat4 & value);
	virtual bool Play(const std::string & name);
};

class AnimationKeyframeInternal : virtual public IAnimationKeyframe, public ObjectInternal {
	DEFINE_FACTORY_METHOD(AnimationKeyframe)

public:
	AnimationKeyframeInternal() :ObjectInternal(ObjectTypeAnimationKeyframe) {}

public:
	virtual void Set(float time, const glm::vec3 & position, const glm::quat & rotation, const glm::vec3 & scale);
};

class AnimationCurveInternal : virtual public IAnimationCurve, public ObjectInternal {
	DEFINE_FACTORY_METHOD(AnimationCurve)
	
public:
	AnimationCurveInternal() :ObjectInternal(ObjectTypeAnimationCurve) {}

public:
	virtual void SetKeyframes(const std::vector<AnimationKeyframe>& keyframes);
	virtual void Sample(float time, glm::vec3 & position, glm::quat & rotation, glm::vec3 & scale);
};
