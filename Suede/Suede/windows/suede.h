#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_suede.h"

class Canvas;
class Console;
class Hierarchy;

class Suede : public QMainWindow {
	Q_OBJECT

public:
	Suede(QWidget *parent = 0);
	~Suede();

public:
	static Suede* get();

public:
	Canvas* canvas() { return ui.canvas; }
	Console* console() { return ui.console; }
	Hierarchy* hierarchy() { return ui.hierarchy; }

protected:
	// TODO: update hierarchy.
	virtual void timerEvent(QTimerEvent *event);
	virtual void keyPressEvent(QKeyEvent *event);

private slots:
	void onEngineLogReceived(int type, const char * message);
	void screenCapture();

private:
	void setupUI();

private:
	int timer_;
	Ui::Suede ui;
};
