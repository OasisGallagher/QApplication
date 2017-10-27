#include "tools/mathf.h"

template <class Cont>
void AnimationKeysInternal::SmoothKey(Cont& container, float time) {
	Cont::value_type key;
	key.time = time;

	Cont::iterator pos = container.find(key);
	if (pos != container.end() && Mathf::Approximately(pos->time, time)) {
		return;
	}

	if (pos == container.end()) {
		key.value = container.back().value;
	}
	else {
		if (pos == container.begin()) {
			key.value = container.front().value;
		}
		else {
			Cont::iterator prev = pos;
			--prev;

			float t = Mathf::Clamp01((time - prev->time) / (pos->time - prev->time));
			key.value = Mathf::Lerp(prev->value, pos->value, t);
		}
	}

	container.insert(key);
}
