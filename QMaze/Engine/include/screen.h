#pragma once
#include "defines.h"
#include "smartptr.h"

class ENGINE_EXPORT IScreen {
public:
	virtual int GetContextWidth() = 0;
	virtual int GetContextHeight() = 0;

	virtual void SetContentSize(int w, int h) = 0;
};

typedef smart_ptr<IScreen> Screen;
