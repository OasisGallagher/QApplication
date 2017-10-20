#include <QMenuBar>
#include <QSplitter>
#include <QKeyEvent>
#include <QFileDialog>

#include "suede.h"
#include "camera.h"
#include "canvas.h"
#include "console.h"

static Suede* instance_;

Suede::Suede(QWidget *parent)
	: QMainWindow(parent) {
	instance_ = this;
	setupUI();

	QMenu* fileMenu = menuBar()->findChild<QMenu*>("file");
	QList<QAction*> actions = fileMenu->actions();

	connect(canvas(), SIGNAL(onEngineLogReceived(int, const char*)), this, SLOT(onEngineLogReceived(int, const char*)));
	connect(actions[0], SIGNAL(triggered()), this, SLOT(screenCapture()));
	connect(actions[1], SIGNAL(triggered()), qApp, SLOT(quit()));

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

void Suede::keyPressEvent(QKeyEvent* event) {
	switch (event->key()) {
		case Qt::Key_Escape:
			qApp->quit();
			break;
	}
}

void Suede::screenCapture() {
	std::vector<Sprite> sprites;
	if (!Engine::get()->world()->GetSprites(ObjectTypeCamera, sprites)) {
		return;
	}

	Camera camera = dsp_cast<Camera>(sprites.front());

	Texture2D tex = camera->Capture();
	std::vector<unsigned char> data;
	if (!tex->EncodeToJpg(data)) {
		return;
	}

	QImage image;
	if (image.loadFromData(&data[0], data.size())) {
		QString filter = "image(*.jpg)";
		QString path = QFileDialog::getSaveFileName(this, "", "", filter);
		if (!path.isEmpty()) {
			image.save(path);
		}
	}
}

void Suede::onEngineLogReceived(int type, const char* message) {
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
