#pragma once
#include "defines.h"

class ENGINE_EXPORT Screen {
public:
	int GetContextWidth();
	int GetContextHeight();

	void SetContentSize(int w, int h);

private:
	friend class Engine;
	Screen() {}

private:
	int width_;
	int height_;
};