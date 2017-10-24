#include "tools/mathf.h"

template <class KeyType>
void AnimationKeysInternal::AddKey(std::vector<KeyType>& container, float time, const typename KeyType::T & value) {
	std::vector<KeyType>::iterator ite = std::lower_bound(container.begin(), container.end(), time, KeyComparer());
	AssertX(ite == container.end() || !Mathf::Approximately(ite->time, time), "duplicate key " + std::to_string(time));

	KeyType key;
	key.time = time, key.value = value;
	container.insert(ite, key);
}

template <class KeyType>
void AnimationKeysInternal::RemoveKey(std::vector<KeyType>& container, float time) {
	std::vector<KeyType>::iterator ite = std::lower_bound(container.begin(), container.end(), time, KeyComparer());
	if (Mathf::Approximately(ite->time, time)) {
		container.erase(ite);
	}
}
