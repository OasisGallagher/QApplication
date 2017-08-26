#include <QList>
#include <QMenu>
#include <QListView>
#include <QKeyEvent>
#include <QMessageBox>
#include <QToolButton>
#include <QFileDialog>
#include <QFileIconProvider>
#include <QtMultimedia/QMediaPlayer>

#include "VideosConfig.h"
#include "setting.h"
#include "defines.h"
#include "configtools.h"
#include "videoeditor.h"
#include "categoryeditor.h"

ConfigTools::ConfigTools(QWidget *parent)
	: QDialog(parent) {
	ui.setupUi(this);
	Qt::WindowFlags flags = windowFlags();
	flags |= Qt::WindowMinimizeButtonHint;
	flags |= Qt::WindowCloseButtonHint;
	flags |= Qt::MSWindowsFixedSizeDialogHint;
	setWindowFlags(flags);

	videoEditor_ = new VideoEditor(this);
	categoryEditor_ = new CategoryEditor(this);

	ui.itemList->horizontalHeader()->setSectionsClickable(false);
	ui.itemList->horizontalHeader()->setStyleSheet("font-weight:bold;");
	ui.itemList->horizontalHeader()->setFixedHeight(25);

	ui.itemList->setSelectionBehavior(QAbstractItemView::SelectRows);
	ui.itemList->setSelectionMode(QAbstractItemView::SingleSelection);
	ui.itemList->setColumnCount(3);

	QStringList headers;
	headers << tr("FileName") << tr("PictureName") << tr("Category");
	ui.itemList->setHorizontalHeaderLabels(headers);

	connect(VideosConfig::get(), SIGNAL(outputModified()), this, SLOT(reloadUIContent()));
	connect(ui.editCategory, SIGNAL(clicked()), this, SLOT(onEditCategory()));

	ui.add->setText(tr("Add%1").arg("..."));
	ui.modify->setText(tr("Modify%1").arg("..."));

	connect(ui.add, SIGNAL(clicked()), this, SLOT(onAdd()));
	connect(ui.modify, SIGNAL(clicked()), this, SLOT(onModify()));
	connect(ui.remove, SIGNAL(clicked()), this, SLOT(onRemove()));

	connect(ui.itemList, SIGNAL(cellDoubleClicked(int, int)), this, SLOT(onItemDoubleClicked(int, int)));
	connect(ui.itemList, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(onTableCustomContextMenuRequested()));

	Setting::get()->read();
	VideosConfig::get()->read();

	reloadUIContent();
}

ConfigTools::~ConfigTools() {
}

void ConfigTools::keyReleaseEvent(QKeyEvent* e) {
	if (e->key() == Qt::Key_Delete) {
		onRemove();
	}
}

void ConfigTools::reloadUIContent() {
	ui.itemList->clearContents();
	ui.itemList->setRowCount(0);

	auto cont = VideosConfig::get()->items();
	for (int i = 0; i < cont.size(); ++i) {
		appendRow(cont[i]);
	}
}

void ConfigTools::appendRow(const VideoItem& item) {
	int p = ui.itemList->rowCount();
	ui.itemList->insertRow(p);
	ui.itemList->setRowHeight(p, 22);

	QFileInfo vi(item.video);
	QFileInfo pi(item.picture);
	QTableWidgetItem* item0 = new QTableWidgetItem(vi.filePath());
	QTableWidgetItem* item1 = new QTableWidgetItem(pi.filePath());
	QTableWidgetItem* item2 = new QTableWidgetItem(item.category);

	ui.itemList->setItem(p, 0, item0);
	ui.itemList->setItem(p, 1, item1);
	ui.itemList->setItem(p, 2, item2);
}

void ConfigTools::onTableCustomContextMenuRequested() {
	QList<QTableWidgetItem*> list = ui.itemList->selectedItems();

	QMenu menu;
	QAction* add = new QAction(tr("Add"), &menu);
	QAction* remove = new QAction(tr("Remove"), &menu);
	QAction* modify = new QAction(tr("Modify"), &menu);

	connect(add, SIGNAL(triggered()), this, SLOT(onAdd()));
	connect(modify, SIGNAL(triggered()), this, SLOT(onModify()));
	connect(remove, SIGNAL(triggered()), this, SLOT(onRemove()));

	if (list.isEmpty()) {
		remove->setEnabled(false);
		modify->setEnabled(false);
	}

	menu.addAction(add);
	menu.addAction(remove);
	menu.addAction(modify);

	menu.exec(QCursor::pos());
}

void ConfigTools::onItemDoubleClicked(int row, int column) {
	onModify();
}

void ConfigTools::onAdd() {
	videoEditor_->exec(-1);
	if (videoEditor_->result() == QDialog::Accepted) {
		reloadUIContent();
	}
}

void ConfigTools::onModify() {
	QList<QTableWidgetItem*> items = ui.itemList->selectedItems();
	if (items.empty()) {
		QMessageBox::warning(this, tr("Warning"), tr("EmptySelectionMessage"), QMessageBox::Ok);
		return;
	}

	QTableWidgetItem* item = items.front();
	videoEditor_->exec(item->row());
	if (videoEditor_->result() == QDialog::Accepted) {
		reloadUIContent();
	}
}

void ConfigTools::onRemove() {
	QList<QTableWidgetItem*> items = ui.itemList->selectedItems();
	if (items.isEmpty()) {
		QMessageBox::warning(this, tr("WarningTitle"), tr("NoRemoveItemPrompt"), QMessageBox::Ok);
		return;
	}

	auto ans = QMessageBox::warning(this, tr("WarningTitle"), tr("RemoveItemPrompt").arg(items.size()), MESSAGE_BUTTON_YES_NO);
	if (ans == QMessageBox::No) {
		return;
	}

	int row = items.front()->row();

	ui.itemList->removeRow(row);
	VideosConfig::get()->remove(row);
}

void ConfigTools::onEditCategory() {
	categoryEditor_->exec();
}
