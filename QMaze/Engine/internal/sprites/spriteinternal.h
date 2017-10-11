#pragma once
#include <bitset>
#include <glm/gtc/quaternion.hpp>

#include "sprite.h"
#include "surface.h"
#include "internal/base/objectinternal.h"

class SpriteInternal : virtual public ISprite, public ObjectInternal {
	DEFINE_FACTORY_METHOD(Sprite)

public:
	SpriteInternal();

public:
	virtual std::string GetName() { return name_; }
	virtual void SetName(const std::string& value) { name_ = value; }

	virtual void AddChild(Sprite child);
	virtual void RemoveChild(Sprite child);

	virtual void SetParent(Sprite value);
	virtual Sprite GetParent() { return parent_.lock(); }

	virtual int GetChildCount() { return (int)children_.size(); }
	virtual Sprite GetChildAt(int i) { return children_[i]; }

public:
	virtual void SetScale(const glm::vec3& value);
	virtual void SetPosition(const glm::vec3& value);
	virtual void SetRotation(const glm::quat& value);
	virtual void SetEulerAngles(const glm::vec3& value);

	virtual glm::vec3 GetScale();
	virtual glm::vec3 GetPosition();
	virtual glm::quat GetRotation();
	virtual glm::vec3 GetEulerAngles();

	virtual void SetLocalScale(const glm::vec3& value);
	virtual void SetLocalPosition(const glm::vec3& value);
	virtual void SetLocalRotation(const glm::quat& value);
	virtual void SetLocalEulerAngles(const glm::vec3& value);

	virtual glm::vec3 GetLocalScale() { return scale_; }
	virtual glm::vec3 GetLocalPosition() { return position_; }
	virtual glm::quat GetLocalRotation() { return rotation_; }
	virtual glm::vec3 GetLocalEulerAngles();

	virtual glm::mat4 GetLocalToWorldMatrix();
	virtual glm::mat4 GetWorldToLocalMatrix();

	virtual glm::vec3 GetLocalToWorldPosition(const glm::vec3& position);
	virtual glm::vec3 GetWorldToLocalPosition(const glm::vec3& position);

	virtual glm::vec3 GetUp();
	virtual glm::vec3 GetRight();
	virtual glm::vec3 GetForward();

	virtual void Update();

	virtual void SetSurface(Surface value){ surface_ = value; }
	virtual Surface GetSurface() { return surface_; }

	virtual void SetRenderer(Renderer value) { renderer_ = value; }
	virtual Renderer GetRenderer() { return renderer_; }

protected:
	SpriteInternal(ObjectType spriteType);

private:
	enum DirtyFlag {
		DirtyFlagWorldScale,
		DirtyFlagWorldRotation,
		DirtyFlagWorldPosition,
		DirtyFlagLocalEulerAngles,
		DirtyFlagWorldEulerAngles,
		DirtyFlagLocalToWorldMatrix,
		DirtyFlagWorldToLocalMatrix,
		DirtyFlagSize,
	};

private:
	bool IsDirty(DirtyFlag bit) { return dirtyFlags_.test(bit); }
	void SetDiry(DirtyFlag bit, bool value) { dirtyFlags_.set(bit, value); }

	const char* SpriteTypeToString(ObjectType type);

private:
	std::string name_;

	Surface surface_;
	Renderer renderer_;
	std::weak_ptr<ISprite> parent_;

	std::vector<Sprite> children_;
	std::bitset<DirtyFlagSize> dirtyFlags_;

	glm::vec3 scale_, worldScale_;
	glm::quat rotation_, worldRotation_;
	glm::vec3 position_, worldPosition_;
	glm::vec3 eulerAngles_, worldEulerAngles_;

	glm::mat4 localToWorldMatrix_;
	glm::mat4 worldToLocalMatrix_;
};
