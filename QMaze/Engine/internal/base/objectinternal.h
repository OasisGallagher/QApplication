#pragma once
#include <vector>
#include <bitset>
#include <glm/glm.hpp>

#include "object.h"
#include "internal/memory/factory.h"

class ObjectInternal : virtual public IObject {
public:
	ObjectInternal(ObjectType type);
	virtual ~ObjectInternal() {}

public:
	virtual unsigned GetInstanceID() const { return id_; }
	virtual ObjectType GetType() const { return type_; }

	virtual std::string GetName() const { return name_; }
	virtual void SetName(const std::string& value) { name_ = value; }

public:
	static void DecodeInstanceID(unsigned value, ObjectType* type, unsigned* id);

private:
	static unsigned EncodeInstanceID(ObjectType type);

private:
	unsigned id_;
	ObjectType type_;
	std::string name_;

	static unsigned ObjectIDContainer[ObjectTypeCount];
};

#define DEFINE_FACTORY_METHOD(name) \
	public: \
		typedef name Interface; \
		static Object Create() { return Factory::Create<name ## Internal>(); } \
	private:
