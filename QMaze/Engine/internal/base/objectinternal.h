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

public:
	static void DecodeInstanceID(unsigned value, ObjectType* type, unsigned* id);

private:
	static unsigned EncodeInstanceID(ObjectType type);

private:
	unsigned id_;
	ObjectType type_;

	static unsigned ObjectIDContainer[ObjectTypeCount];
};

#define DEFINE_FACTORY_METHOD(name) \
	public: \
		typedef name Interface; \
		static Object Create() { return Factory::Create<name ## Internal>(); } \
	private:
