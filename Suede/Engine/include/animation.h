#pragma once
#include "object.h"

class ENGINE_EXPORT IAnimationClip : virtual public IObject {
};

class ENGINE_EXPORT IAnimationState : virtual public IObject {
};

class ENGINE_EXPORT IAnimation : virtual public IObject {
};

typedef std::shared_ptr<IAnimation> Animation;
typedef std::shared_ptr<IAnimationClip> AnimationClip;
typedef std::shared_ptr<IAnimationState> AnimationState;
