#pragma once

#include <QtOpenGL/QGLWidget>

class GLWidget : public QGLWidget {
	Q_OBJECT
public:
	GLWidget(QWidget *parent = NULL);

protected:
	void initializeGL();
	void resizeGL(int w, int h);
	void paintGL();

	void wheelEvent(QWheelEvent* event);
	void mousePressEvent(QMouseEvent *event);
	void mouseMoveEvent(QMouseEvent *event);
	void mouseReleaseEvent(QMouseEvent* event);
	void keyPressEvent(QKeyEvent *event);

private:
	bool mpressed_;
	bool lpressed;

	QPoint mpos_;
	QPoint lpos_;
};
