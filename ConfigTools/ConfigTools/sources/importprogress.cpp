#include "importprogress.h"
#include "ui_importprogress.h"

ImportProgress::ImportProgress(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::ImportProgress) {
	ui->setupUi(this);
	setWindowFlags(windowFlags() & ~Qt::WindowCloseButtonHint | Qt::MSWindowsFixedSizeDialogHint);
}

ImportProgress::~ImportProgress() {
	delete ui;
}

void ImportProgress::setText(const QString& text) {
	ui->label->setText(text);
}
