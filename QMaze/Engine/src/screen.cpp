#include "screen.h"

int Screen::GetContextWidth() {
	return width_;
}

int Screen::GetContextHeight() {
	return height_;
}

void Screen::SetContentSize(int w, int h) {
	width_ = w;
	height_ = h;
}
