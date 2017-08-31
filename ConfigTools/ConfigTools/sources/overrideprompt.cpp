#include <QUrl>
#include <QDesktopServices>

#include <QFileInfo>
#include "utility.h"
#include "overrideprompt.h"
#include "ui_OverridePrompt.h"

OverridePrompt::OverridePrompt(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::OverridePrompt) {
	ui->setupUi(this);

	Qt::WindowFlags flags = windowFlags();
	flags |= Qt::WindowMinimizeButtonHint;
	flags |= Qt::WindowCloseButtonHint;
	flags |= Qt::MSWindowsFixedSizeDialogHint;
	flags &= ~Qt::WindowCloseButtonHint;

	ui->button0->setStyleSheet("font-family: Microsoft YaHei;");
	ui->button1->setStyleSheet("font-family: Microsoft YaHei;");
	ui->button2->setStyleSheet("font-family: Microsoft YaHei;");

	setWindowFlags(flags);

	connect(ui->button0, SIGNAL(clicked()), this, SLOT(onCommand()));
	connect(ui->button1, SIGNAL(clicked()), this, SLOT(onCommand()));
	connect(ui->button2, SIGNAL(clicked()), this, SLOT(onCommand()));
	connect(ui->position, SIGNAL(linkActivated(QString)), this, SLOT(openDirectory(QString)));
}

OverridePrompt::~OverridePrompt() {
	delete ui;
}

void OverridePrompt::openDirectory(QString path) {
	QDesktopServices::openUrl(QUrl(path, QUrl::TolerantMode));
}

void OverridePrompt::onCommand() {
	QObject* ptr = sender();
	int ans = (ptr == ui->button0) ? 0 : (ptr == ui->button1 ? 1 : 2);
	done(ans);
}

void OverridePrompt::closeEvent(QCloseEvent*) {
	done(0);
}

int OverridePrompt::exec(const QString& path, bool& rememberMyChoise) {
	QFileInfo info(path);
	ui->position->setText(QString("<a href='file:///%1'>%2</a>").arg(info.path()).arg(Utility::elidedText(ui->position, info.path())));
	ui->fileName->setText(Utility::elidedText(ui->fileName, info.fileName()));
	ui->button2->setDescription(tr("SaveAs ").arg(Utility::alternativeFileName(path)));
	int ans = QDialog::exec();
	rememberMyChoise = ui->remember->isChecked();
	return ans;
}
