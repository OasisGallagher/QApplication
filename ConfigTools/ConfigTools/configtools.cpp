#include <QListView>
#include <QMessageBox>
#include <QFileDialog>
#include <QFileIconProvider>

#include "configtools.h"

ConfigTools::ConfigTools(QWidget *parent)
	: QDialog(parent) {
	ui.setupUi(this);
	Qt::WindowFlags flags = windowFlags();
	flags |= Qt::WindowMinimizeButtonHint;
	flags |= Qt::WindowCloseButtonHint;
	flags |= Qt::MSWindowsFixedSizeDialogHint;
	setWindowFlags(flags);

	model_ = new QStandardItemModel;
	ui.itemList->setModel(model_);

	QStringList headers;
	headers << tr("FileName") << tr("PictureName");
	model_->setHorizontalHeaderLabels(headers);

	if (setting_.open()) {
		ui.categories->addItems(setting_.videoCategories());
	}

	if (output_.open()) {
		QFileIconProvider ip;
		foreach(OutputItem item, output_.items()) {
			QFileInfo vi(item.video);
			QFileInfo pi(item.picture);

			QStandardItem* p = new QStandardItem(/*ip.icon(vi), */vi.fileName());
			model_->appendRow(p);

			QStandardItem* q = new QStandardItem(/*ip.icon(pi), */pi.fileName());
			model_->setItem(model_->indexFromItem(p).row(), 1, q);
		}
	}

	ui.modify->setEnabled(false);

	connect(ui.selectVideo, SIGNAL(clicked()), this, SLOT(onAddVideo()));
	connect(ui.selectPicture, SIGNAL(clicked()), this, SLOT(onAddPicture()));
	connect(ui.save, SIGNAL(clicked()), this, SLOT(onSave()));

	connect(ui.videoPath, SIGNAL(textChanged(const QString&)), this, SLOT(onVideoPathChanged(const QString&)));
	connect(ui.picturePath, SIGNAL(textChanged(const QString&)), this, SLOT(onPicturePathChanged(const QString&)));
}

void ConfigTools::onSave() {
	output_.flush();
	output_.clear();
}

void ConfigTools::onAddVideo() {
	QString file = QFileDialog::getOpenFileName(this, tr("SelectVideo"), "", tr("Videos") + " (" + setting_.videoPrefix() + ")");
	if (file.isEmpty()) { return; }
	ui.videoPath->setText(file);
}

void ConfigTools::onAddPicture() {
	QString file = QFileDialog::getOpenFileName(this, tr("SelectVideo"), "", tr("Videos") + " (" + setting_.picturePrefix() + ")");
	if (file.isEmpty()) { return; }
	ui.picturePath->setText(file);
}

void ConfigTools::onVideoPathChanged(const QString& path) {
	QFile file(path);
	QString ff = "font-family: Microsoft Yahei;";
	ui.videoPath->setStyleSheet(ff + (path.isEmpty() || file.exists() ? "color:black;" : "color:red;"));
}

void ConfigTools::onPicturePathChanged(const QString& path) {
	QFile file(path);
	QString ff = "font-family: Microsoft Yahei;";
	ui.picturePath->setStyleSheet(ff + (path.isEmpty() || file.exists() ? "color:black;" : "color:red;"));
}
