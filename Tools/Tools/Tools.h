#pragma once

#include <QtWidgets/QDialog>
#include "ui_Tools.h"

class USBTools;

class Tools : public QDialog
{
	Q_OBJECT

public:
	Tools(QWidget *parent = Q_NULLPTR);
	~Tools();

protected:
	virtual bool nativeEvent(const QByteArray &eventType, void *message, long *result);

private:
	USBTools* usb_;

	Ui::Tools ui;
};
