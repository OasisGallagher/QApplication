#include "worldinternal.h"
#include "internal/memory/factory.h"
#include "internal/sprites/spriteinternal.h"
#include "internal/sprites/camerainternal.h"

bool WorldInternal::SpriteComparer::operator() (Sprite lhs, Sprite rhs)const {
	ObjectType lt = lhs->GetType(), rt = rhs->GetType();
	if (lt == rt) {
		// TODO: opaque or transparent...
		return lhs->GetInstanceID() < rhs->GetInstanceID();
	}

	return lt != ObjectTypeCamera;
}

void WorldInternal::Update() {
	for (SpriteContainer::iterator ite = sprites_.begin(); ite != sprites_.end(); ++ite) {
		(*ite)->Update();
	}
}

Camera WorldInternal::AddCamera() {
	Camera camera = dynamic_ptr_cast<Camera>(Factory::Create<CameraInternal>());
	sprites_.insert(camera);

	return camera;
}

Sprite WorldInternal::AddSprite() {
	Sprite sprite = dynamic_ptr_cast<Sprite>(Factory::Create<SpriteInternal>());
	sprites_.insert(sprite);
	return sprite;
}
