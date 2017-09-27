#include "qmaze.h"

QMaze* QMaze::instance_;

QMaze::QMaze(QWidget *parent)
	: QMainWindow(parent) {
	ui.setupUi(this);
	instance_ = this;
}

QMaze::~QMaze() {
}

void QMaze::addConsoleMessage(const QString& message) {
	ui.console->append(message);
}
