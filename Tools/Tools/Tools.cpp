#include <QDebug>
#include <QMessageBox>

#include <QDirModel>

#include "Tools.h"
#include "USBTools.h"

Tools::Tools(QWidget *parent)
	: QDialog(parent) {
	ui.setupUi(this);

	usb_ = new USBTools;
	usb_->scan();

	QDirModel* model = new QDirModel();
	ui.fileTree->setModel(model);
	ui.fileTree->setRootIndex(model->index("c:/"));
}

Tools::~Tools() {
	delete usb_;
}

bool Tools::nativeEvent(const QByteArray &eventType, void *message, long *result) {
	Q_UNUSED(eventType);

	usb_->onWinEvent((MSG*)message, result);

	return false;
}
