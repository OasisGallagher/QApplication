#include <QMessageBox>

#include "clienteditor.h"
#include "ui_clienteditor.h"
#include "clientconfig.h"

ClientEditor::ClientEditor(QWidget *parent) :
	QDialog(parent), modifying_(-1),
	ui(new Ui::ClientEditor) {
	ui->setupUi(this);

	connect(ui->add, SIGNAL(clicked()), this, SLOT(onAdd()));
	connect(ui->cancel, SIGNAL(clicked()), this, SLOT(reject()));
}

ClientEditor::~ClientEditor() {
	delete ui;
}

void ClientEditor::exec(int modifying) {
	modifying_ = modifying;

	ui->ip->clear();

	if (modifying_ >= 0) {
		const ClientItem& item = ClientConfig::get()->itemAt(modifying_);
		ui->ip->setText(item.ip);
	}

	ui->add->setText(modifying_ >= 0 ? tr("Modify") : tr("Add"));

	QDialog::exec();
}

void ClientEditor::onAdd() {
	QString ip = ui->ip->text();

	if (ip.isEmpty()) {
		Message::warning(this, tr("InvalidIpAddress"));
		return;
	}

	ClientItem tmp(ip);
	if (modifying_ >= 0) {
		modifyItem(tmp);
	}
	else {
		addItem(tmp);
	}
}

void ClientEditor::addItem(const ClientItem& item) {
	if (ClientConfig::get()->add(item)) {
		accept();
	}
}

void ClientEditor::modifyItem(const ClientItem& to) {
	if (ClientConfig::get()->modify(modifying_, to)) {
		accept();
	}
}
