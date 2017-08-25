#include "configtools.h"

#include <QTextCodec>
#include <QTranslator>
#include <QtWidgets/QApplication>

#pragma execution_character_set("utf-8")

int main(int argc, char *argv[]) {
	QApplication a(argc, argv);

	QTranslator translator;
	translator.load(":/lang/zh");
	a.installTranslator(&translator);
	
	ConfigTools w;
	w.show();
	return a.exec();
}
