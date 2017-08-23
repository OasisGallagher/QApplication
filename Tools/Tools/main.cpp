#include "Tools.h"

#include <QTextCodec>
#include <QTranslator>
#include <QtOpenGL/QGLFormat>
#include <QtWidgets/QApplication>

int main(int argc, char *argv[]) {
	QApplication a(argc, argv);

	QTextCodec *codec = QTextCodec::codecForName("System");
	QTextCodec::setCodecForLocale(codec);

	QTranslator translator;
	translator.load(":/lang/zh");
	a.installTranslator(&translator);
	/*
	QSurfaceFormat format;
	format.setRenderableType(QSurfaceFormat::OpenGL);
	//format.setDepthBufferSize(24);
	//format.setStencilBufferSize(8);
	format.setVersion(3, 3);
	format.setProfile(QSurfaceFormat::CompatibilityProfile);
	auto version = format.version();
	format.setOption(QSurfaceFormat::DebugContext);
	QSurfaceFormat::setDefaultFormat(format);
	*/
	Tools w;
	w.show();

	return a.exec();
}
