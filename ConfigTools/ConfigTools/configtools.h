#pragma once

#include <QtWidgets/QDialog>
#include <QStandardItemModel>

#include "ui_configtools.h"
#include "output.h"
#include "setting.h"

class ConfigTools : public QDialog {
	Q_OBJECT

public:
	ConfigTools(QWidget *parent = Q_NULLPTR);

private slots:
	void onSave();
	void onAddVideo();
	void onAddPicture();

	void onVideoPathChanged(const QString& path);
	void onPicturePathChanged(const QString& path);
private:
	Output output_;
	Setting setting_;
	Ui::ConfigTools ui;

	QStandardItemModel* model_;
};
