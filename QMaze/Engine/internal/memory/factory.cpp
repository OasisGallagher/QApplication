#include "internal/memory/factory.h"

#include "internal/world/worldinternal.h"

Factory Factory::instance;

Factory::Factory() {
	AddFactoryMethod("World", WorldInternal::Create);
	AddFactoryMethod("Camera", CameraInternal::Create)
}