#include <QtGui>

#include "qtDDraw.h"

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	QTextCodec::setCodecForTr(QTextCodec::codecForName("gb18030"));

	QtDDraw w;
	w.show();
	return a.exec();
}
