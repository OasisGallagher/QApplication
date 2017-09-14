#include "internal/memory/factory.h"

#include "internal/base/meshinternal.h"
#include "internal/world/worldinternal.h"
#include "internal/base/surfaceinternal.h"
#include "internal/base/textureinternal.h"
#include "internal/base/materialinternal.h"
#include "internal/sprites/camerainternal.h"

Factory Factory::instance;

Factory::Factory() {
	AddFactoryMethod("World", WorldInternal::Create);
	AddFactoryMethod("Camera", CameraInternal::Create);
	AddFactoryMethod("Sprite", SpriteInternal::Create);
	AddFactoryMethod("Surface", SurfaceInternal::Create);
	AddFactoryMethod("Material", MaterialInternal::Create);
	AddFactoryMethod("Mesh", MeshInternal::Create);
	AddFactoryMethod("Texture2D", Texture2DInternal::Create);
	AddFactoryMethod("Texture3D", Texture3DInternal::Create);
}
