#include <QtGui>

#include "myDDraw.h"
#include "qtDDraw.h"

#include <windows.h>

QtDDraw::QtDDraw(QWidget *parent /* = 0 */, Qt::WFlags flags /* = 0 */)
:QWidget(parent, flags)
{
	setWindowTitle(tr("using DDraw in Qt"));
	videoWidget = new QWidget(this);
	videoWidget->setFixedSize(QSize(640,480));

	m_ddraw = new MyDDraw(CWnd::FromHandle(videoWidget->winId()));
	m_timer = new QTimer(this); 
	connect(m_timer, SIGNAL(timeout()), this, SLOT(refreshScreen()));
	m_timer->start(40);

	QVBoxLayout *layout = new QVBoxLayout;
	layout->setContentsMargins(0,0,0,0);
	layout->addWidget(videoWidget);

	setLayout(layout);
}

QtDDraw::~QtDDraw()
{

}

void QtDDraw::refreshScreen()
{		
	m_ddraw->Update_Screen();
	//m_timer->stop();
}