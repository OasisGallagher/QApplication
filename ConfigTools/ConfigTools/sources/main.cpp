#include "configtools.h"

#include <QTextCodec>
#include <QTranslator>
#include <QtWidgets/QApplication>

int main(int argc, char *argv[]) {
	QApplication a(argc, argv);
	
	QTranslator translator;
	translator.load(":/languages/zh");
	a.installTranslator(&translator);

	ConfigTools w;
	w.show();
	return a.exec();
}
