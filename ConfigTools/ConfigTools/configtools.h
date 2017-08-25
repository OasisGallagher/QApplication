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
	~ConfigTools();

private slots:
	void onAdd();
	void onRemove();

	void onBrowseVideo();
	void onEditCategory();
	void onBrowsePicture();

	void onVideoPathChanged(const QString& path);
	void onPicturePathChanged(const QString& path);

	void onListItemSelectionChanged();
	void onListItemDoubleClicked(int row, int column);
	
	void reloadUIContent();
	void reloadComboBox();

private:
	void appendRow(const OutputItem& item);

private:
	bool saved_;
	Ui::ConfigTools ui;
};
