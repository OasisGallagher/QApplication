#pragma once
#include <time.h>

class Time {
public:
	Time();

public:
	float GetDeltaTime();
	float GetRealTimeSinceStartup();
	int GetFrameCount();
	void Update();

private:
	int frames_;
	clock_t clocks_;
	float deltaTime_;
};
