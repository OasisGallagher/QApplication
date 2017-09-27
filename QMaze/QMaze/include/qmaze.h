#ifndef QMAZE_H
#define QMAZE_H

#include <QtWidgets/QMainWindow>
#include "ui_qmaze.h"

class QMaze : public QMainWindow
{
	Q_OBJECT

public:
	QMaze(QWidget *parent = 0);
	~QMaze();

public:
	// TODO: Console class.
	static QMaze* get() { return instance_; }
	void addConsoleMessage(const QString& message);

private:
	static QMaze* instance_;

private:
    Ui::QMaze ui;
};

#endif // QMAZE_H
