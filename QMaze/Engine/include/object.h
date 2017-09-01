#pragma once
#include <vector>
#include <bitset>
#include <glm/glm.hpp>

#include "defs.h"

class ENGINE_EXPORT Object {
public:
	Object(ObjectType type);

	unsigned GetInstanceID() const { return id_; }
	int GetType() const { return type_; }

	std::string GetName() const { return name_; }
	void SetName(const std::string& value) { name_ = value; }

public:
	static void DecodeInstanceID(unsigned value, ObjectType* type, unsigned* id);

private:
	static unsigned EncodeInstanceID(ObjectType type);

private:
	unsigned id_;
	ObjectType type_;
	std::string name_;

	static unsigned short ObjectIDContainer[ObjectTypeCount];
};
