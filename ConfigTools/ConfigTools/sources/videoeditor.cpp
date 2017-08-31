#include <QFile>
#include <QFileDialog>

#include "defines.h"
#include "setting.h"
#include "utility.h"
#include "videoconfig.h"
#include "videoeditor.h"

VideoEditor::VideoEditor(QWidget *parent) : QDialog(parent), modifying_(-1) {
	ui.setupUi(this);
	/*
	ui.gridLayout->removeWidget(ui.name);
	ui.gridLayout->addWidget(ui.name, 0, 1, 1, 2);
	*/
	ui.gridLayout->removeWidget(ui.categories);
	ui.gridLayout->addWidget(ui.categories, 3, 1, 1, 2);

	connect(ui.selectVideo, SIGNAL(clicked()), this, SLOT(onBrowseVideo()));
	connect(ui.selectPicture, SIGNAL(clicked()), this, SLOT(onBrowsePicture()));

	connect(ui.videoPath, SIGNAL(textChanged(const QString&)), this, SLOT(onVideoPathChanged(const QString&)));
	connect(ui.picturePath, SIGNAL(textChanged(const QString&)), this, SLOT(onPicturePathChanged(const QString&)));

	connect(ui.add, SIGNAL(clicked()), this, SLOT(onAdd()));
	connect(ui.cancel, SIGNAL(clicked()), this, SLOT(reject()));

	connect(ui.autoFill, SIGNAL(stateChanged(int)), this, SLOT(onAutoFillStateChanged()));
}

void VideoEditor::exec(int modifying) {
	modifying_ = modifying;

	ui.videoPath->clear();
	ui.picturePath->clear();
	ui.categories->clear();
	ui.categories->addItems(Setting::get()->categories());
	ui.categories->setCurrentIndex(-1);
	ui.autoFill->setChecked(false);
	//ui.name->setText(tr("AutoFillFromVideoPath"));
	//ui.name->setEnabled(false);

	if (modifying_ >= 0) {
		const VideoItem& item = VideoConfig::get()->itemAt(modifying_);
		ui.name->setText(item.name);
		ui.videoPath->setText(item.video);
		ui.picturePath->setText(item.picture);

		int index = Setting::get()->categories().indexOf(item.category);
		ui.categories->setCurrentIndex(index);
	}

	ui.add->setText(modifying_ >= 0 ? tr("Modify") : tr("Add"));
	QDialog::exec();
}

void VideoEditor::onAdd() {
	QString videoPath = ui.videoPath->text();
	if (videoPath.isEmpty() || !isFileExists(videoPath, VIDEO_PATH)) {
		Message::warning(this, tr("InvalidVideoPathMessage"));
		return;
	}

	QString videoName = ui.name->text();
	if (videoName.isEmpty()) {
		videoName = QFileInfo(videoPath).baseName();
	}

	QString picturePath = ui.picturePath->text();
	if (picturePath.isEmpty() || !isFileExists(picturePath, PICTURE_PATH)) {
		Message::warning(this, tr("InvalidPicturePathMessage"));
		return;
	}

	int index = ui.categories->currentIndex();
	if (index < 0) {
		Message::warning(this, tr("InvalidCategorySelectedMessage"));
		return;
	}

	VideoItem tmp(videoName, videoPath.replace('\\', '/'), picturePath.replace('\\', '/'), Setting::get()->categoryAt(index));
	if (modifying_ >= 0) {
		modifyItem(tmp);
	}
	else {
		addItem(tmp);
	}
}

void VideoEditor::addItem(const VideoItem& item) {
	if (VideoConfig::get()->add(item)) {
		accept();
	}
}

void VideoEditor::modifyItem(const VideoItem& to) {
	if (VideoConfig::get()->modify(modifying_, to)) {
		accept();
	}
}

void VideoEditor::onBrowseVideo() {
	QString file = QFileDialog::getOpenFileName(this, tr("SelectVideo"), VIDEO_PATH, tr("Videos") + " (" + Setting::get()->videoPrefix() + ")");
	if (file.isEmpty()) { return; }

	QDir dir(VIDEO_PATH);
	file = dir.relativeFilePath(file);
	ui.videoPath->setText(file);
	if (ui.autoFill->isChecked()) {
		ui.name->setText(QFileInfo(file).baseName());
	}
}

void VideoEditor::onBrowsePicture() {
	QString file = QFileDialog::getOpenFileName(this, tr("SelectVideo"), PICTURE_PATH, tr("Pictures") + " (" + Setting::get()->picturePrefix() + ")");
	if (file.isEmpty()) { return; }

	QDir dir(PICTURE_PATH);
	file = dir.relativeFilePath(file);
	ui.picturePath->setText(file);
}

void VideoEditor::onVideoPathChanged(const QString& path) {
	QString ff = "font-family: Microsoft Yahei; font-size:13px;";
	ui.videoPath->setStyleSheet(ff + (path.isEmpty() || isFileExists(path, VIDEO_PATH) ? "color:black;" : "color:red;"));
}

void VideoEditor::onPicturePathChanged(const QString& path) {
	QString ff = "font-family: Microsoft Yahei; font-size:13px;";
	ui.picturePath->setStyleSheet(ff + (path.isEmpty() || isFileExists(path, PICTURE_PATH) ? "color:black;" : "color:red;"));
}

void VideoEditor::onAutoFillStateChanged() {
	ui.name->clear();

	if (ui.autoFill->isChecked()) {
		QString path = ui.videoPath->text();
		if (path.isEmpty()) {
			ui.name->setText(tr("AutoFillFromVideoPath"));
		}
		else {
			ui.name->setText(QFileInfo(path).baseName());
		}
	}

	ui.name->setEnabled(!ui.autoFill->isChecked());
}

bool VideoEditor::isFileExists(const QString& path, const QString& base) {
	if (path.isEmpty()) { return false; }
	QDir dir(base);
	QFile file(dir.absoluteFilePath(path));
	return file.exists();
}
