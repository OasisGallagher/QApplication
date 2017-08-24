#pragma once

#include <QtWidgets/QDialog>
#include "ui_configtools.h"

class ConfigTools : public QDialog
{
	Q_OBJECT

public:
	ConfigTools(QWidget *parent = Q_NULLPTR);

private slots:
	void onAddVideo();
	void onVideoPathChanged(const QString& path);

private:
	Ui::ConfigTools ui;
};
