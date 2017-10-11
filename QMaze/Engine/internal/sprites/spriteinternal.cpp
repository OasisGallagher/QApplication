#include <algorithm>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>

#include "tools/mathf.h"
#include "tools/string.h"
#include "internal/sprites/spriteinternal.h"

SpriteInternal::SpriteInternal() : ObjectInternal(ObjectTypeSprite), scale_(1) {
	dirtyFlags_.set();
	name_ = String::Format("%s (%u)", SpriteTypeToString(GetType()), GetInstanceID());
}

SpriteInternal::SpriteInternal(ObjectType spriteType) : ObjectInternal(spriteType), scale_(1) {
	AssertX(spriteType > ObjectTypeSprite && spriteType < ObjectTypeCount, "invalid sprite type " + std::to_string(spriteType));
	dirtyFlags_.set();
	name_ = String::Format("%s (%u)", SpriteTypeToString(GetType()), GetInstanceID());
}

void SpriteInternal::AddChild(Sprite child) {
	if (std::find(children_.begin(), children_.end(), child) == children_.end()) {
		children_.push_back(child);
	}
}

void SpriteInternal::RemoveChild(Sprite child) {
	children_.erase(std::remove(children_.begin(), children_.end(), child), children_.end());
}

void SpriteInternal::SetParent(Sprite value) {
	std::weak_ptr<ISprite> old = parent_;
	// old parent.
	Sprite sprite = old.lock();
	Sprite thisSp = dsp_cast<Sprite>(shared_from_this());
	if (sprite) {
		sprite->RemoveChild(thisSp);
	}

	parent_ = value;
	sprite = parent_.lock();
	if (sprite) {
		sprite->AddChild(thisSp);
	}
}

void SpriteInternal::SetScale(const glm::vec3& value) {
	Sprite current = dsp_cast<Sprite>(shared_from_this());
	glm::vec3 scale = value;
	if ((current = current->GetParent())) {
		scale /= current->GetScale();
	}

	SetLocalScale(scale);
}

void SpriteInternal::SetPosition(const glm::vec3& value) {
	Sprite current = dsp_cast<Sprite>(shared_from_this());
	glm::vec3 position = value;
	if ((current = current->GetParent())) {
		position -= current->GetPosition();
	}

	SetLocalPosition(position);
}

void SpriteInternal::SetRotation(const glm::quat& value) {
	Sprite current = dsp_cast<Sprite>(shared_from_this());
	glm::quat rotation = value;
	if ((current = current->GetParent())) {
		rotation *= glm::inverse(current->GetRotation());
	}

	SetLocalRotation(rotation);
}

void SpriteInternal::SetEulerAngles(const glm::vec3& value) {
	SetLocalRotation(glm::quat(glm::vec3(Mathf::Radians(value.x), Mathf::Radians(value.y), Mathf::Radians(value.z))));
}

glm::vec3 SpriteInternal::GetScale() {
	if (IsDirty(DirtyFlagWorldScale)) {
		Sprite current = dsp_cast<Sprite>(shared_from_this());
		glm::vec3 scale = GetLocalScale();
		if ((current = current->GetParent())) {
			scale *= current->GetScale();
		}

		worldScale_ = scale;
		SetDiry(DirtyFlagWorldScale, false);
	}

	return worldScale_;
}

glm::vec3 SpriteInternal::GetPosition() {
	if (IsDirty(DirtyFlagWorldPosition)) {
		Sprite current = dsp_cast<Sprite>(shared_from_this());
		glm::vec3 position = GetLocalPosition();
		if ((current = current->GetParent())) {
			position += current->GetPosition();
		}

		worldPosition_ = position;
		SetDiry(DirtyFlagWorldPosition, false);
	}

	return worldPosition_;
}

glm::quat SpriteInternal::GetRotation() {
	if (IsDirty(DirtyFlagWorldRotation)) {
		Sprite current = dsp_cast<Sprite>(shared_from_this());
		glm::quat rotation = GetLocalRotation();
		if ((current = current->GetParent())) {
			rotation *= current->GetRotation();
		}

		worldRotation_ = rotation;
		SetDiry(DirtyFlagWorldRotation, false);
	}

	return worldRotation_;
}

glm::vec3 SpriteInternal::GetEulerAngles() {
	if (IsDirty(DirtyFlagWorldEulerAngles)) {
		glm::vec3 angles = glm::eulerAngles(GetRotation());
		worldEulerAngles_ = glm::vec3(Mathf::Degrees(angles.x), Mathf::Degrees(angles.y), Mathf::Degrees(angles.z));
		SetDiry(DirtyFlagWorldEulerAngles, false);
	}

	return worldEulerAngles_;
}

void SpriteInternal::SetLocalScale(const glm::vec3& value) {
	if (scale_ == value) { return; }
	scale_ = value;
	SetDiry(DirtyFlagWorldScale, true);
	SetDiry(DirtyFlagLocalToWorldMatrix, true);
	SetDiry(DirtyFlagWorldToLocalMatrix, true);
}

void SpriteInternal::SetLocalPosition(const glm::vec3& value) {
	if (position_ == value) { return; }
	position_ = value;
	SetDiry(DirtyFlagWorldPosition, true);
	SetDiry(DirtyFlagLocalToWorldMatrix, true);
	SetDiry(DirtyFlagWorldToLocalMatrix, true);
}

void SpriteInternal::SetLocalRotation(const glm::quat& value) {
	if (Mathf::Approximately(glm::dot(rotation_, value), 0)) { return; }
	rotation_ = value;
	SetDiry(DirtyFlagWorldRotation, true);
	SetDiry(DirtyFlagLocalToWorldMatrix, true);
	SetDiry(DirtyFlagWorldToLocalMatrix, true);
}

void SpriteInternal::SetLocalEulerAngles(const glm::vec3& value) {
	SetLocalRotation(glm::quat(value));
}

inline glm::vec3 SpriteInternal::GetLocalEulerAngles() {
	if (IsDirty(DirtyFlagLocalEulerAngles)) {
		eulerAngles_ = glm::eulerAngles(rotation_);
		SetDiry(DirtyFlagLocalEulerAngles, false);
	}

	return eulerAngles_;
}

glm::mat4 SpriteInternal::GetLocalToWorldMatrix() {
	if (IsDirty(DirtyFlagLocalToWorldMatrix)) {
		Sprite current = dsp_cast<Sprite>(shared_from_this());
		glm::mat4 matrix = glm::translate(glm::mat4(1), GetLocalPosition()) * glm::toMat4(GetLocalRotation()) * glm::scale(glm::mat4(1), GetLocalScale());
		if ((current = current->GetParent())) {
			matrix = current->GetLocalToWorldMatrix() * matrix;
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

glm::vec3 SpriteInternal::GetUp() {
	return rotation_ * glm::vec3(0, 1, 0);
}

glm::vec3 SpriteInternal::GetRight() {
	return rotation_ * glm::vec3(1, 0, 0);
}

glm::vec3 SpriteInternal::GetForward() {
	return rotation_ * glm::vec3(0, 0, -1);
}

void SpriteInternal::Update() {

}

const char* SpriteInternal::SpriteTypeToString(ObjectType type) {
	const char* name = "";
	switch (type) {
		case ObjectTypeSprite:
			name = "Sprite";
			break;
		case ObjectTypeCamera:
			name = "Camera";
			break;
		case ObjectTypeSkybox:
			name = "Skybox";
			break;
		case ObjectTypeSpotLight:
			name = "SpotLight";
			break;
		case ObjectTypePointLight:
			name = "PointLight";
			break;
		case ObjectTypeDirectionalLight:
			name = "DirectionalLight";
			break;
	}

	return name;
}
