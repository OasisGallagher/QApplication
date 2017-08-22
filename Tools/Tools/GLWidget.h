#pragma once
#include <QWidget>

class GLWidget : public QWidget {
public:
	GLWidget(QWidget* parent = Q_NULLPTR);
	~GLWidget();

public:
	virtual void paintEvent(QPaintEvent *event);
	virtual QPaintEngine* paintEngine();

private:
	HDC dc_;
};
