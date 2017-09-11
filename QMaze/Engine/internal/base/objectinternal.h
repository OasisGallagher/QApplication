#pragma once
#include <vector>
#include <bitset>
#include <glm/glm.hpp>

#include "object.h"

#define DEFINE_FACTORY_METHOD(Class) \
	public: \
		static Object* Create() { return Factory::Create<Class ## Internal>(); } \
	private:

class ObjectInternal : virtual public Object {
public:
	ObjectInternal(ObjectType type);
	virtual ~ObjectInternal() {}

public:
	virtual unsigned GetInstanceID() const { return id_; }
	virtual int GetType() const { return type_; }

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

