#pragma once
#include "engine.h"

#include <QTextBrowser>
#include <QtOpenGL/QGLWidget>

class PostEffect;
class CameraController;

class Canvas : public QGLWidget, public EngineLogCallback {
	Q_OBJECT
public:
	Canvas(QWidget *parent = NULL);
	~Canvas();

public:
	virtual void OnEngineLogMessage(int level, const char * message);

protected:
	void initializeGL();
	void resizeGL(int w, int h);
	void paintGL();

	void timerEvent(QTimerEvent *event);
	void wheelEvent(QWheelEvent* event);
	void mousePressEvent(QMouseEvent *event);
	void mouseMoveEvent(QMouseEvent *event);
	void mouseReleaseEvent(QMouseEvent* event);
	void keyPressEvent(QKeyEvent *event);

private:
	void createScene();

signals:
	void onEngineLogReceived(int level, const char* message);

private:
	int timer_;
	bool sceneCreated_;
	PostEffect* grayscale_;
	PostEffect* inversion_;
	CameraController* controller_;
};
