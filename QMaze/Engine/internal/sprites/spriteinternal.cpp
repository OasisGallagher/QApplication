#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>

#include "tools/mathf.h"
#include "internal/sprites/spriteinternal.h"

SpriteInternal::SpriteInternal() : ObjectInternal(ObjectTypeSprite), scale_(1) {
	dirtyFlags_.set();
}

SpriteInternal::SpriteInternal(ObjectType spriteType) : ObjectInternal(spriteType), scale_(1) {
	AssertX(spriteType > ObjectTypeSprite, "invalid sprite type " + std::to_string(spriteType));
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

void SpriteInternal::SetRotation(const glm::quat& value) {
	if (Mathf::Approximately(glm::dot(rotation_, value), 0)) { return; }
	rotation_ = value;
	SetDiry(DirtyFlagLocalToWorldMatrix, true);
	SetDiry(DirtyFlagWorldToLocalMatrix, true);
}

void SpriteInternal::SetEulerAngles(const glm::vec3& value) {
	SetRotation(glm::quat(value));
}

glm::mat4 SpriteInternal::GetLocalToWorldMatrix() {
	if (IsDirty(DirtyFlagLocalToWorldMatrix)) {
		glm::mat4 ans(1.f);
		localToWorldMatrix_ = glm::translate(ans, position_) * glm::toMat4(rotation_) * glm::scale(ans, scale_);
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

glm::vec3 SpriteInternal::GetUp() const {
	return rotation_ * glm::vec3(0, 1, 0);
}

glm::vec3 SpriteInternal::GetRight() const {
	return rotation_ * glm::vec3(1, 0, 0);
}

glm::vec3 SpriteInternal::GetForward() const {
	return rotation_ * glm::vec3(0, 0, -1);
}

void SpriteInternal::Update() {

}
