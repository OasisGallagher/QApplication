#pragma once

#include <QStandardItem>
#include <QtWidgets/QDialog>

#include "ui_Tools.h"

#include "USBTools.h"

enum {
	IconFile,
	IconDirectory,
	IconCount,
};

class Tools : public QDialog
{
	Q_OBJECT

public:
	Tools(QWidget *parent = Q_NULLPTR);
	~Tools();

protected:
	virtual bool nativeEvent(const QByteArray &eventType, void *message, long *result);

private slots:
	void onTreeItemChanged(QStandardItem* item);
	void onDiskAdded(const USBDisk& disk);
	void onDiskRemoved(const USBDisk& disk);
	void onVideosContextMenu(const QPoint& point);
	void onToggleVideosView();

private:
	void updateTree(const USBDisk& disk);
	void updateVideosModel(const USBDisk& disk);

	void checkAllChild(QStandardItem* item, bool check);
	void onChildCheckStateChanged(QStandardItem* item);
	Qt::CheckState checkSibling(QStandardItem* item);

	void __testTree();
	bool initItemModel(QString path, QStandardItemModel* model, QStandardItem *item);

private:
	USBTools* usb_;
	QIcon icons_[IconCount];
	Ui::Tools ui;

	QStandardItemModel* tree_model_;
	QStandardItemModel* videos_model_;
};
