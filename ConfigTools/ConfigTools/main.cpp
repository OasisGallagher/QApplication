#include "configtools.h"

#include <QTextCodec>
#include <QtWidgets/QApplication>

#pragma execution_character_set("utf-8")

int main(int argc, char *argv[]) {
	QApplication a(argc, argv);
	
	ConfigTools w;
	w.show();
	return a.exec();
}
