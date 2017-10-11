#include "timef.h"

#define ClockToSecond(clocks)	(float(clocks) / CLOCKS_PER_SEC)

Time::Time() :clocks_(0), frames_ (0){
}

void Time::Update() {
	clock_t now = clock();
	deltaTime_ = ClockToSecond(now - clocks_);
	clocks_ = now;

	++frames_;
}

float Time::GetDeltaTime() {
	return deltaTime_;
}

float Time::GetRealTimeSinceStartup() {
	return ClockToSecond(clock());
}

int Time::GetFrameCount() {
	return frames_;
}
