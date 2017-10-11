#pragma once
#include <time.h>
#include "defines.h"

class ENGINE_EXPORT Time {
public:
	float GetDeltaTime();
	float GetRealTimeSinceStartup();
	int GetFrameCount();
	void Update();

private:
	friend class Engine;
	Time();

private:
	int frames_;
	clock_t clocks_;
	float deltaTime_;
};
