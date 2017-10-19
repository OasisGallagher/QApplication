#include <QSplitter>

#include "suede.h"
#include "canvas.h"
#include "console.h"

static Suede* instance_;

Suede::Suede(QWidget *parent)
	: QMainWindow(parent) {
	instance_ = this;

	setupUI();

	connect(canvas(), SIGNAL(onEngineLogReceived(int, const char*)), this, SLOT(OnEngineLogReceived(int, const char*)));

	timer_ = startTimer(1000);
}

Suede::~Suede() {
}

Suede* Suede::get() {
	return instance_;
}

void Suede::setupUI() {
	ui.setupUi(this);

	QWidget* cw = takeCentralWidget();
	cw->deleteLater();

	console()->initialize();

	QSplitter* hs = new QSplitter(Qt::Horizontal, this);
	QSplitter* vs = new QSplitter(Qt::Vertical, this);

	vs->addWidget(canvas());
	vs->addWidget(console());
	vs->setStretchFactor(0, 20);
	vs->setStretchFactor(1, 1);

	hs->addWidget(vs);
	hs->addWidget(hierarchy());
	hs->setStretchFactor(0, 10);
	hs->setStretchFactor(1, 1);

	setCentralWidget(hs);
}

void Suede::timerEvent(QTimerEvent *event) {
	if (event->timerId() != timer_) { return; }
	std::vector<Sprite> sprites;
	hierarchy()->update(Engine::get()->world()->GetRootSprite());
	killTimer(timer_);
}

void Suede::OnEngineLogReceived(int type, const char* message) {
	switch (type) {
		case 0:
			console()->addMessage(Console::Debug, message);
			break;

		case 1:
			console()->addMessage(Console::Warning, message);
			break;

		case 2:
		case 3:
			console()->addMessage(Console::Error, message);
			if (type == 3) { qFatal(message); }
			break;
	}
}
