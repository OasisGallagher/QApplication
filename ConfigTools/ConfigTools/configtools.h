#pragma once

#include <QtWidgets/QDialog>
#include <QStandardItemModel>

#include "ui_configtools.h"

struct VideoItem;
class VideoEditor;
class CategoryEditor;

class ConfigTools : public QDialog {
	Q_OBJECT

public:
	ConfigTools(QWidget *parent = Q_NULLPTR);
	~ConfigTools();

protected:
	virtual void keyReleaseEvent(QKeyEvent* e);

private slots:
	void onAdd();
	void onModify();
	void onRemove();
	void onEditCategory();

	void onTableCustomContextMenuRequested();
	void onItemDoubleClicked(int row, int column);
	
	void reloadUIContent();
	
private:
	void appendRow(const VideoItem& item);

private:
	VideoEditor* videoEditor_;
	CategoryEditor* categoryEditor_;

	Ui::ConfigTools ui;
};
