#pragma once
#include <glm/gtc/quaternion.hpp>

#include "sprite.h"
#include "surface.h"
#include "internal/base/objectinternal.h"

class SpriteInternal : virtual public ISprite, public ObjectInternal {
	DEFINE_FACTORY_METHOD(Sprite)

public:
	SpriteInternal();

public:
	virtual void SetParent(Sprite value) { parent_ = value; }
	virtual Sprite GetParent() const { return parent_; }

	virtual int GetChildCount() const { return (int)children_.size(); }
	virtual Sprite GetChildAt(int i) { return children_[i]; }

public:
	virtual void SetScale(const glm::vec3& value);
	virtual void SetPosition(const glm::vec3& value);
	virtual void SetRotation(const glm::quat& value);
	virtual void SetEulerAngles(const glm::vec3& value);

	virtual glm::vec3 GetScale() const { return scale_; }
	virtual glm::vec3 GetPosition() const { return position_; }
	virtual glm::quat GetRotation() const { return rotation_; }
	virtual glm::vec3 GetEulerAngles() const { return glm::eulerAngles(rotation_); }

	virtual glm::mat4 GetLocalToWorldMatrix();
	virtual glm::mat4 GetWorldToLocalMatrix();

	virtual glm::vec3 GetLocalToWorldPosition(const glm::vec3& position);
	virtual glm::vec3 GetWorldToLocalPosition(const glm::vec3& position);

	virtual glm::vec3 GetUp() const;
	virtual glm::vec3 GetRight() const;
	virtual glm::vec3 GetForward() const;

	virtual void Update();

	virtual void SetSurface(Surface value){ surface_ = value; }
	virtual Surface GetSurface() { return surface_; }

	virtual void SetRenderer(Renderer value) { renderer_ = value; }
	virtual Renderer GetRenderer() { return renderer_; }

protected:
	SpriteInternal(ObjectType spriteType);

private:
	enum DirtyFlag {
		DirtyFlagLocalToWorldMatrix,
		DirtyFlagWorldToLocalMatrix,
		DirtyFlagSize,
	};

private:
	bool IsDirty(DirtyFlag bit) const { return dirtyFlags_.test(bit); }
	void SetDiry(DirtyFlag bit, bool value) { dirtyFlags_.set(bit, value); }

private:
	Sprite parent_;
	Surface surface_;
	Renderer renderer_;

	std::vector<Sprite> children_;
	std::bitset<DirtyFlagSize> dirtyFlags_;

	glm::vec3 scale_;
	glm::quat rotation_;
	glm::vec3 position_;

	glm::mat4 localToWorldMatrix_;
	glm::mat4 worldToLocalMatrix_;
};
