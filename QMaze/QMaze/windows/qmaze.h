#ifndef QMAZE_H
#define QMAZE_H

#include <QtWidgets/QMainWindow>
#include "ui_qmaze.h"

class Canvas;
class Console;
class Hierarchy;

class QMaze : public QMainWindow {
	Q_OBJECT

public:
	QMaze(QWidget *parent = 0);
	~QMaze();

public:
	Canvas* canvas() { return ui.canvas; }
	Console* console() { return ui.console; }
	Hierarchy* hierarchy() { return ui.hierarchy; }

protected:
	// TODO: update hierarchy.
	virtual void timerEvent(QTimerEvent *event);

private slots:
	void OnEngineLogReceived(int type, const char * message);

private:
	void setupUI();

private:
	int timer_;
	Ui::QMaze ui;
};

#endif // QMAZE_H
