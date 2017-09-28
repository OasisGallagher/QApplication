#include "tools/mathf.h"
#include "worldinternal.h"
#include "internal/memory/factory.h"
#include "internal/world/environmentinternal.h"

bool WorldInternal::LightComparer::operator()(const Light & lhs, const Light & rhs) const {
	// Directional light > Importance > Luminance.
	ObjectType lt = lhs->GetType(), rt = rhs->GetType();
	if (lt != rt && (lt == ObjectTypeDirectionalLight || rt == ObjectTypeDirectionalLight)) {
		return lt == ObjectTypeDirectionalLight;
	}

	LightImportance lli = lhs->GetImportance(), rli = rhs->GetImportance();
	if (lli != rli) {
		return lli > rli;
	}

	return Mathf::Luminance(lhs->GetColor()) > Mathf::Luminance(rhs->GetColor());
}

bool WorldInternal::CameraComparer::operator() (const Camera& lhs, const Camera& rhs) const {
	return lhs->GetDepth() < rhs->GetDepth();
}

bool WorldInternal::SpriteComparer::operator() (const Sprite& lhs, const Sprite& rhs) const {
	return lhs->GetInstanceID() < rhs->GetInstanceID();
}

WorldInternal::WorldInternal() : ObjectInternal(ObjectTypeWorld), environment_(Memory::Create<EnvironmentInternal>()) {
}

Object WorldInternal::Create(ObjectType type) {
	Object object = Factory::Create(type);
	if (type >= ObjectTypeSprite) {
		sprites_.push_back(dynamic_sp_cast<Sprite>(object));
	}

	if (type >= ObjectTypeSpotLight && type <= ObjectTypeDirectionalLight) {
		lights_.insert(dynamic_sp_cast<Light>(object));
	}

	if (type == ObjectTypeCamera) {
		cameras_.insert(dynamic_sp_cast<Camera>(object));
	}

	return object;
}

bool WorldInternal::GetSprites(ObjectType type, std::vector<Sprite>& sprites) {
	AssertX(type >= ObjectTypeSprite, "invalid sprite type");
	if (type == ObjectTypeSprite) {
		sprites.assign(sprites_.begin(), sprites_.end());
	}
	else if (type == ObjectTypeCamera) {
		sprites.assign(cameras_.begin(), cameras_.end());
	}
	else if (type == ObjectTypeLight) {
		sprites.assign(lights_.begin(), lights_.end());
	}
	else {
		for (SpriteContainer::iterator ite = sprites_.begin(); ite != sprites_.end(); ++ite) {
			if ((*ite)->GetType() == type) {
				sprites.push_back(*ite);
			}
		}
	}

	return !sprites.empty();
}

void WorldInternal::Update() {
	for (SpriteContainer::iterator ite = sprites_.begin(); ite != sprites_.end(); ++ite) {
		(*ite)->Update();
	}

	for (CameraContainer::iterator ite = cameras_.begin(); ite != cameras_.end(); ++ite) {
		(*ite)->Render();
	}
}
