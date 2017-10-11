#include "screeninternal.h"

int ScreenInternal::GetContextWidth() {
	return width_;
}

int ScreenInternal::GetContextHeight() {
	return height_;
}

void ScreenInternal::SetContentSize(int w, int h) {
	width_ = w;
	height_ = h;
}
