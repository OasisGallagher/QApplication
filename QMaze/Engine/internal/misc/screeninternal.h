#include "screen.h"

class ScreenInternal : public IScreen {
public:
	virtual int GetContextWidth();
	virtual int GetContextHeight();

	virtual void SetContentSize(int w, int h);

private:
	int width_;
	int height_;
};