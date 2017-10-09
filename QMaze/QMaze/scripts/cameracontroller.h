#pragma once
#include "camera.h"
#include <QMouseEvent>

class CameraController {
public:
	CameraController();

public:
	void setCamera(Camera value);
	void onMouseWheel(int delta);
	void onMouseMove(const QPoint& pos);
	void onMouseRelease(Qt::MouseButton button);
	void onMousePress(Qt::MouseButton button, const QPoint& pos);

private:
	Camera camera_;

	bool mpressed_;
	bool rpressed;

	QPoint mpos_;
	QPoint rpos_;
};
