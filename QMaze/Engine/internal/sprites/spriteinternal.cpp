#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

#include "internal/sprites/spriteinternal.h"

SpriteInternal::SpriteInternal(Sprite* parent) : ObjectInternal(ObjectTypeSprite) {
	parent_ = parent;
	dirtyFlags_.set();
}

void SpriteInternal::SetScale(const glm::vec3& value) {
	if (scale_ == value) { return; }
	scale_ = value;
	SetDiry(DirtyFlagLocalToWorldMatrix, true);
	SetDiry(DirtyFlagWorldToLocalMatrix, true);
}

void SpriteInternal::SetPosition(const glm::vec3& value) {
	if (position_ == value) { return; }
	position_ = value;
	SetDiry(DirtyFlagLocalToWorldMatrix, true);
	SetDiry(DirtyFlagWorldToLocalMatrix, true);
}

void SpriteInternal::SetEulerAngles(const glm::vec3& value) {
	if (eulerAngles_ == value) { return; }
	eulerAngles_ = value;
	SetDiry(DirtyFlagLocalToWorldMatrix, true);
	SetDiry(DirtyFlagWorldToLocalMatrix, true);
}

glm::mat4 SpriteInternal::GetLocalToWorldMatrix() {
	if (IsDirty(DirtyFlagLocalToWorldMatrix)) {
		glm::mat4 ans(1.f);
		glm::quat q(eulerAngles_);
		localToWorldMatrix_ = glm::translate(ans, position_) * glm::toMat4(q) * glm::scale(ans, scale_);
		SetDiry(DirtyFlagLocalToWorldMatrix, false);
	}

	return localToWorldMatrix_;
}

glm::mat4 SpriteInternal::GetWorldToLocalMatrix() {
	if (IsDirty(DirtyFlagWorldToLocalMatrix)) {
		worldToLocalMatrix_ = glm::inverse(GetLocalToWorldMatrix());
		SetDiry(DirtyFlagWorldToLocalMatrix, false);
	}

	return worldToLocalMatrix_;
}

glm::vec3 SpriteInternal::GetLocalToWorldPosition(const glm::vec3& position) {
	return glm::vec3(GetLocalToWorldMatrix() * glm::vec4(position, 1));
}

glm::vec3 SpriteInternal::GetWorldToLocalPosition(const glm::vec3& position) {
	return glm::vec3(GetWorldToLocalMatrix() * glm::vec4(position, 1));
}
