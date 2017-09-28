#include "internal/memory/factory.h"

#include "internal/base/meshinternal.h"
#include "internal/base/shaderinternal.h"
#include "internal/world/worldinternal.h"
#include "internal/base/surfaceinternal.h"
#include "internal/base/textureinternal.h"
#include "internal/base/rendererinternal.h"
#include "internal/base/materialinternal.h"
#include "internal/sprites/lightinternal.h"
#include "internal/sprites/skyboxinternal.h"
#include "internal/sprites/camerainternal.h"

Factory Factory::instance;

#define ADD_FACTROY_METHOD(name) \
	AddFactoryMethod(#name, name ## Internal::Create);  \
	AddFactoryMethod(ObjectType ## name, name ## Internal::Create)

Factory::Factory() {
	std::fill(methodArray_, methodArray_ + ObjectTypeCount, nullptr);

	ADD_FACTROY_METHOD(Mesh);
	ADD_FACTROY_METHOD(World);
	ADD_FACTROY_METHOD(Camera);
	ADD_FACTROY_METHOD(Shader);
	ADD_FACTROY_METHOD(Skybox);
	ADD_FACTROY_METHOD(Sprite);
	ADD_FACTROY_METHOD(Surface);
	ADD_FACTROY_METHOD(Material);
	ADD_FACTROY_METHOD(Renderer);
	ADD_FACTROY_METHOD(Texture2D);
	ADD_FACTROY_METHOD(TextureCube);
	ADD_FACTROY_METHOD(RenderTexture);

	ADD_FACTROY_METHOD(SpotLight);
	ADD_FACTROY_METHOD(PointLight);
	ADD_FACTROY_METHOD(DirectionalLight);
}
