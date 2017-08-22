#ifndef QT_DDRAW_H
#define QT_DDRAW_H

#include <QWidget>

class MyDDraw;

class QtDDraw: public QWidget
{
	Q_OBJECT

public:
	QtDDraw(QWidget *parent = 0, Qt::WFlags flags = 0);
	~QtDDraw();

private:
	QWidget *videoWidget;

	QTimer *m_timer;
	MyDDraw *m_ddraw;

public slots:
	void refreshScreen();
};

#endif // QT_DDRAW_H