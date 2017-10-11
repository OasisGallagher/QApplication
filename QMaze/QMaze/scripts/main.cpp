#include "windows/qmaze.h"
#include <QtWidgets/QApplication>
#include <memory>

int main(int argc, char *argv[]) {
	QApplication a(argc, argv);
	QMaze w;
	w.show();
	return a.exec();
}
