#include "Tools.h"

#include <QTextCodec>
#include <QTranslator>
#include <QtWidgets/QApplication>

int main(int argc, char *argv[]) {
	QApplication a(argc, argv);

	QTextCodec *codec = QTextCodec::codecForName("System");
	QTextCodec::setCodecForLocale(codec);

	QTranslator translator;
	translator.load(":/lang/zh");
	a.installTranslator(&translator);

	Tools w;
	w.show();
	return a.exec();
}
