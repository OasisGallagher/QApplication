#include "worldinternal.h"
#include "internal/memory/factory.h"
#include "internal/sprites/spriteinternal.h"
#include "internal/sprites/camerainternal.h"

WorldInternal::WorldInternal() : ObjectInternal(ObjectTypeWorld) {
}

bool WorldInternal::SpriteComparer::operator() (Sprite lhs, Sprite rhs)const {
	ObjectType lt = lhs->GetType(), rt = rhs->GetType();
	if (lt == rt) {
		// TODO: opaque or transparent...
		return lhs->GetInstanceID() < rhs->GetInstanceID();
	}

	return lt != ObjectTypeCamera;
}

Object WorldInternal::Create(const std::string& type) {
	Object object = Factory::Create(type);
	if (object->GetType() >= ObjectTypeSprite) {
		sprites_.insert(dynamic_ptr_cast<Sprite>(object));
	}

	return object;
}

bool WorldInternal::CollectSprites(std::vector<Sprite>* sprites, float fieldOfView, float aspect, float nearClipPlane, float farClipPlane) {
	sprites->insert(sprites->end(), sprites_.begin(), sprites_.end());
	return true;
}

void WorldInternal::Update() {
	for (SpriteContainer::iterator ite = sprites_.begin(); ite != sprites_.end(); ++ite) {
		(*ite)->Update();
	}
}
