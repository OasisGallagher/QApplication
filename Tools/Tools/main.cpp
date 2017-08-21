#include "Tools.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	Tools w;
	w.show();
	return a.exec();
}
