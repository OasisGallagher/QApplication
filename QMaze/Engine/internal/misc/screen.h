#pragma once
class Screen {
public:
	int GetContextWidth();
	int GetContextHeight();

	void SetContentSize(int w, int h);

private:
	int width_;
	int height_;
};