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

void SpriteInternal::SetScale(const glm::vec3 & value) {
	glm::vec3 scale = value;
	for (ISprite* parent; (parent = GetParent().get()) != nullptr; ) {
		scale /= parent->GetLocalScale();
	}

	SetLocalScale(scale);
}

void SpriteInternal::SetPosition(const glm::vec3 & value) {
	glm::vec3 position = value;
	for (ISprite* parent; (parent = GetParent().get()) != nullptr; ) {
		position -= parent->GetLocalPosition();
	}

	SetLocalPosition(position);
}

void SpriteInternal::SetRotation(const glm::quat & value) {
	glm::quat rotation = value;
	for (ISprite* parent; (parent = GetParent().get()) != nullptr; ) {
		rotation *= glm::inverse(parent->GetLocalRotation());
	}

	SetLocalRotation(rotation);
}

void SpriteInternal::SetEulerAngles(const glm::vec3 & value) {
	SetLocalRotation(glm::quat(value));
}

glm::vec3 SpriteInternal::GetScale() const {
	glm::vec3 scale(1);
	for (const ISprite* current = this; current != nullptr; current = GetParent().get()) {
		scale *= current->GetLocalScale();
	}

	return scale;
}

glm::vec3 SpriteInternal::GetPosition() const {
	glm::vec3 position(0);
	for (const ISprite* current = this; current != nullptr; current = GetParent().get()) {
		position += current->GetLocalPosition();
	}

	return position;
}

glm::quat SpriteInternal::GetRotation() const {
	glm::quat rotation;
	for (const ISprite* current = this; current != nullptr; current = GetParent().get()) {
		rotation *= current->GetLocalRotation();
	}

	return rotation;
}

glm::vec3 SpriteInternal::GetEulerAngles() const {
	return glm::eulerAngles(GetRotation());
}

void SpriteInternal::SetLocalScale(const glm::vec3& value) {
	if (scale_ == value) { return; }
	scale_ = value;
	SetDiry(DirtyFlagLocalToWorldMatrix, true);
	SetDiry(DirtyFlagWorldToLocalMatrix, true);
}

void SpriteInternal::SetLocalPosition(const glm::vec3& value) {
	if (position_ == value) { return; }
	position_ = value;
	SetDiry(DirtyFlagLocalToWorldMatrix, true);
	SetDiry(DirtyFlagWorldToLocalMatrix, true);
}

void SpriteInternal::SetLocalRotation(const glm::quat& value) {
	if (Mathf::Approximately(glm::dot(rotation_, value), 0)) { return; }
	rotation_ = value;
	SetDiry(DirtyFlagLocalToWorldMatrix, true);
	SetDiry(DirtyFlagWorldToLocalMatrix, true);
}

void SpriteInternal::SetLocalEulerAngles(const glm::vec3& value) {
	SetLocalRotation(glm::quat(value));
}

glm::mat4 SpriteInternal::GetLocalToWorldMatrix() {
	if (IsDirty(DirtyFlagLocalToWorldMatrix)) {
		glm::mat4 matrix = glm::mat4(1.f);
		for (ISprite* current = this; current != nullptr; current = GetParent().get()) {
			matrix = glm::translate(matrix, position_) * glm::toMat4(rotation_) * glm::scale(matrix, scale_) * matrix;
		}

		localToWorldMatrix_ = matrix;
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
