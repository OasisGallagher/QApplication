#pragma once
#include "sprite.h"
#include "internal/base/objectinternal.h"

class SpriteInternal : public Sprite, public ObjectInternal {
public:
	SpriteInternal(Sprite* parent = nullptr);

public:
	virtual void SetParent(Sprite* value) { parent_ = value; }
	virtual Sprite* GetParent() const { return parent_; }

	virtual int GetChildCount() const { return (int)children_.size(); }
	virtual Sprite* GetChildAt(int i) const { return children_[i]; }

public:
	virtual void SetScale(const glm::vec3& value);
	virtual void SetPosition(const glm::vec3& value);
	virtual void SetEulerAngles(const glm::vec3& value);

	virtual glm::vec3 GetScale() const { return scale_; }
	virtual glm::vec3 GetPosition() const { return position_; }
	virtual glm::vec3 GetEulerAngles() const { return eulerAngles_; }

	virtual glm::mat4 GetLocalToWorldMatrix();
	virtual glm::mat4 GetWorldToLocalMatrix();

	virtual glm::vec3 GetLocalToWorldPosition(const glm::vec3& position);
	virtual glm::vec3 GetWorldToLocalPosition(const glm::vec3& position);

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
	Sprite* parent_;
	std::vector<Sprite*> children_;
	std::bitset<DirtyFlagSize> dirtyFlags_;

	glm::vec3 scale_;
	glm::vec3 eulerAngles_;
	glm::vec3 position_;

	glm::mat4 localToWorldMatrix_;
	glm::mat4 worldToLocalMatrix_;
};
