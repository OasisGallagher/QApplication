#pragma once
#include <QtOpenGL/QGLWidget>

class GLWidget : public QGLWidget {
public:
	GLWidget(QWidget* parent = Q_NULLPTR);
	~GLWidget() {}

protected:
	virtual void initializeGL();
	virtual void resizeGL(int w, int h);
	virtual void paintGL();
	virtual void mousePressEvent(QMouseEvent *event);
	virtual void mouseMoveEvent(QMouseEvent *event);
	virtual void keyPressEvent(QKeyEvent *event);
};
