#pragma once
#include "object.h"

class ENGINE_EXPORT Sprite : public Object {
public:
	Sprite(int type, Sprite* parent);

public:
	void SetParent(Object* value) { parent_ = value; }
	Object* GetParent() const { return parent_; }

	int GetChildCount() const { return (int)children_.size(); }
	Object* GetChildAt(int i) const { return children_[i]; }

public:
	void SetScale(const glm::vec3& value);
	void SetPosition(const glm::vec3& value);
	void SetEulerAngles(const glm::vec3& value);

	glm::vec3 GetScale() const { return scale_; }
	glm::vec3 GetPosition() const { return position_; }
	glm::vec3 GetEulerAngles() const { return eulerAngles_; }

	glm::mat4 GetLocalToWorldMatrix();
	glm::mat4 GetWorldToLocalMatrix();

	glm::vec3 GetLocalToWorldPosition(const glm::vec3& position);
	glm::vec3 GetWorldToLocalPosition(const glm::vec3& position);

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
	Object* parent_;
	std::vector<Object*> children_;
	std::bitset<DirtyFlagSize> dirtyFlags_;

	glm::vec3 scale_;
	glm::vec3 eulerAngles_;
	glm::vec3 position_;

	glm::mat4 localToWorldMatrix_;
	glm::mat4 worldToLocalMatrix_;
};
