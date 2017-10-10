#include <QSplitter>

#include "qmaze.h"
#include "canvas.h"
#include "console.h"

QMaze::QMaze(QWidget *parent)
	: QMainWindow(parent) {
	setupUI();

	console()->initialize();
	connect(canvas(), SIGNAL(onEngineLogReceived(int, const char*)), this, SLOT(OnEngineLogReceived(int, const char*)));

	timer_ = startTimer(1);
}

QMaze::~QMaze() {
}

void QMaze::setupUI() {
	ui.setupUi(this);

	QWidget* cw = takeCentralWidget();
	cw->deleteLater();

	QSplitter* hs = new QSplitter(Qt::Horizontal, this);
	QSplitter* vs = new QSplitter(Qt::Vertical, this);

	vs->addWidget(ui.canvas);
	vs->addWidget(console());
	vs->setStretchFactor(0, 20);
	vs->setStretchFactor(1, 1);

	hs->addWidget(vs);
	hs->addWidget(ui.hierarchy);
	hs->setStretchFactor(0, 10);
	hs->setStretchFactor(1, 1);

	setCentralWidget(hs);
}

void QMaze::timerEvent(QTimerEvent *event) {
	if (event->timerId() != timer_) { return; }
	std::vector<Sprite> sprites;
	if (Engine::get()->world()->GetSprites(ObjectTypeSprite, sprites)) {
		ui.hierarchy->update(sprites);
		killTimer(timer_);
	}
}

void QMaze::OnEngineLogReceived(int type, const char* message) {
	switch (type) {
		case 0:
			console()->addMessage(Console::Debug, message);
			qDebug(message);
			break;

		case 1:
			console()->addMessage(Console::Warning, message);
			qWarning(message);
			break;

		case 2:
		case 3:
			console()->addMessage(Console::Error, message);
			if (type == 3) { qFatal(message); }
			break;
	}
}
