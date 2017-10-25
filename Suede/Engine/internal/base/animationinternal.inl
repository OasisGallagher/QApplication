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

template <class KeyType>
void AnimationKeysInternal::SmoothKey(std::vector<KeyType>& container, float time) {
	std::vector<KeyType>::iterator pos = std::lower_bound(container.begin(), container.end(), time, KeyComparer());
	if (pos != container.end() && Mathf::Approximately(pos->time, time)) {
		return;
	}

	KeyType key;
	key.time = time;
	if (pos == container.end()) {
		key.value = container.back().value;
	}
	else {
		if (pos == container.begin()) {
			key.value = container.front().value;
		}
		else {
			std::vector<KeyType>::iterator prev = pos;
			--prev;

			key.value = Mathf::Lerp(prev->value, pos->value, time - prev->time / (pos->time - prev->time));
		}
	}

	container.insert(pos, key);
}
