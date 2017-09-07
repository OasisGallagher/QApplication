#include "debug.h"
#include "object.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

ObjectPrivate::ObjectPrivate(ObjectType type) {
	id_ = EncodeInstanceID(type);
	type_ = type;
}

unsigned ObjectPrivate::EncodeInstanceID(ObjectType type) {
	Assert(ObjectIDContainer[type] < std::numeric_limits<unsigned short>::max());
	return Utility::MakeDword(++ObjectIDContainer[type], type);
}

void ObjectPrivate::DecodeInstanceID(unsigned value, ObjectType* type, unsigned* id) {
	if (type != nullptr) { *type = (ObjectType)Utility::Highword(value); }
	if (id != nullptr) { *id = Utility::Loword(value); }
}
