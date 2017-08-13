#include <QTextCodec>
#include <QTranslator>
#include <QtWidgets/QApplication>

#include "Application.h"

#pragma execution_character_set("utf-8")

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);

	QTextCodec *codec = QTextCodec::codecForName("System");
	QTextCodec::setCodecForLocale(codec);

	QTranslator translator;
	translator.load(":/qm/zh");
	a.installTranslator(&translator);

	Application w;
	w.show();
	return a.exec();
}

