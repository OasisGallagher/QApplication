#include <QFile>
#include <QFileDialog>
#include <QMessageBox>

#include "VideosConfig.h"
#include "defines.h"
#include "setting.h"
#include "videoeditor.h"

VideoEditor::VideoEditor(QWidget *parent) : QDialog(parent), modifying_(-1) {
	ui.setupUi(this);

	ui.gridLayout->removeWidget(ui.categories);
	ui.gridLayout->addWidget(ui.categories, 2, 1, 1, 2);
	ui.gridLayout->setEnabled(false);

	connect(ui.selectVideo, SIGNAL(clicked()), this, SLOT(onBrowseVideo()));
	connect(ui.selectPicture, SIGNAL(clicked()), this, SLOT(onBrowsePicture()));

	connect(ui.videoPath, SIGNAL(textChanged(const QString&)), this, SLOT(onVideoPathChanged(const QString&)));
	connect(ui.picturePath, SIGNAL(textChanged(const QString&)), this, SLOT(onPicturePathChanged(const QString&)));

	connect(ui.add, SIGNAL(clicked()), this, SLOT(onAdd()));
	connect(ui.cancel, SIGNAL(clicked()), this, SLOT(reject()));
}

void VideoEditor::exec(int modifying) {
	modifying_ = modifying;

	ui.videoPath->clear();
	ui.picturePath->clear();
	ui.categories->clear();
	ui.categories->addItems(Setting::get()->categories());
	ui.categories->setCurrentIndex(-1);

	if (modifying_ >= 0) {
		const VideoItem& item = VideosConfig::get()->itemAt(modifying_);
		ui.videoPath->setText(modifying_ >= 0 ? item.video : "");
		ui.picturePath->setText(modifying_ >= 0 ? item.picture : "");

		int index = Setting::get()->categories().indexOf(item.category);
		ui.categories->setCurrentIndex(index);
	}

	ui.add->setText(modifying_ >= 0 ? tr("Modify") : tr("Add"));
	QDialog::exec();
}

void VideoEditor::onAdd() {
	QString videoPath = ui.videoPath->text();
	if (videoPath.isEmpty() || !isFileExists(videoPath)) {
		QMessageBox::warning(this, tr("WarningTitle"), tr("InvalidVideoPathMessage"), QMessageBox::Ok);
		return;
	}

	QString picturePath = ui.picturePath->text();
	if (picturePath.isEmpty() || !isFileExists(picturePath)) {
		QMessageBox::warning(this, tr("WarningTitle"), tr("InvalidPicturePathMessage"), QMessageBox::Ok);
		return;
	}

	int index = ui.categories->currentIndex();
	if (index < 0) {
		QMessageBox::warning(this, tr("WarningTitle"), tr("InvalidCategorySelectedMessage"), QMessageBox::Ok);
		return;
	}

	VideoItem tmp(videoPath.replace('\\', '/'), picturePath.replace('\\', '/'), Setting::get()->categoryAt(index));
	if (modifying_ >= 0) {
		modifyItem(tmp);
	}
	else {
		addItem(tmp);
	}
}

void VideoEditor::addItem(const VideoItem& item) {
	if (VideosConfig::get()->add(item)) {
		accept();
	}
}

void VideoEditor::modifyItem(const VideoItem& to) {
	if (VideosConfig::get()->modify(modifying_, to)) {
		accept();
	}
}

void VideoEditor::onBrowseVideo() {
	QString file = QFileDialog::getOpenFileName(this, tr("SelectVideo"), DATA_PATH, tr("Videos") + " (" + Setting::get()->videoPrefix() + ")");
	if (file.isEmpty()) { return; }

	QDir dir(DATA_PATH);
	file = dir.relativeFilePath(file);
	ui.videoPath->setText(file);
}

void VideoEditor::onBrowsePicture() {
	QString file = QFileDialog::getOpenFileName(this, tr("SelectVideo"), DATA_PATH, tr("Videos") + " (" + Setting::get()->picturePrefix() + ")");
	if (file.isEmpty()) { return; }

	QDir dir(DATA_PATH);
	file = dir.relativeFilePath(file);
	ui.picturePath->setText(file);
}

void VideoEditor::onVideoPathChanged(const QString& path) {
	QString ff = "font-family: Microsoft Yahei; font-size:13px;";
	ui.videoPath->setStyleSheet(ff + (path.isEmpty() || isFileExists(path) ? "color:black;" : "color:red;"));
}

void VideoEditor::onPicturePathChanged(const QString& path) {
	QString ff = "font-family: Microsoft Yahei; font-size:13px;";
	ui.picturePath->setStyleSheet(ff + (path.isEmpty() || isFileExists(path) ? "color:black;" : "color:red;"));
}

bool VideoEditor::isFileExists(const QString& path) {
	if (path.isEmpty()) { return false; }
	QDir dir(DATA_PATH);
	QFile file(dir.absoluteFilePath(path));
	return file.exists();
}
