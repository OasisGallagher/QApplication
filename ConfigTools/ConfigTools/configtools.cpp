#include <QMessageBox>
#include <QFileDialog>

#include "configtools.h"

ConfigTools::ConfigTools(QWidget *parent)
	: QDialog(parent) {
	ui.setupUi(this);
	Qt::WindowFlags flags = windowFlags();
	flags |= Qt::WindowMinimizeButtonHint;
	flags |= Qt::WindowCloseButtonHint;
	flags |= Qt::MSWindowsFixedSizeDialogHint;
	setWindowFlags(flags);

	ui.modify->setVisible(false);

	connect(ui.selectVideo, SIGNAL(clicked()), this, SLOT(onAddVideo()));

	connect(ui.videoPath, SIGNAL(textChanged(const QString&)), this, SLOT(onVideoPathChanged(const QString&)));
}

void ConfigTools::onAddVideo() {
	QString file = QFileDialog::getOpenFileName(this, tr("SelectVideo"), "", "Videos (*.mp4 *.avi *.flv)");
	if (file.isEmpty()) { return; }
	ui.videoPath->setText(file);
}

void ConfigTools::onVideoPathChanged(const QString& path) {
	QFile file(path);
	ui.videoPath->setStyleSheet(file.exists() ? "color:black;" : "color:red;");
}
