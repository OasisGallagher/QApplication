#pragma once

#include <QStandardItem>
#include <QtWidgets/QDialog>

#include "ui_Tools.h"

class USBTools;

enum {
	IconFirst,
	IconSecond,
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
	void treeItemChanged(QStandardItem* item);
	
private:
	void checkAllChild(QStandardItem* item, bool check);
	void onChildCheckStateChanged(QStandardItem* item);
	Qt::CheckState checkSibling(QStandardItem* item);

	void __testTree();
	bool __testShowFiles(QString path, QStandardItemModel* model, QStandardItem *item);

private:
	USBTools* usb_;
	QIcon icons_[IconCount];
	Ui::Tools ui;
};
