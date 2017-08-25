#include <QList>
#include <QDebug>
#include <QListView>
#include <QMessageBox>
#include <QToolButton>
#include <QFileDialog>
#include <QFileIconProvider>
#include <QtMultimedia/QMediaPlayer>

#include "defines.h"
#include "configtools.h"
#include "categoryeditor.h"

ConfigTools::ConfigTools(QWidget *parent)
	: QDialog(parent), saved_(true) {
	ui.setupUi(this);
	Qt::WindowFlags flags = windowFlags();
	flags |= Qt::WindowMinimizeButtonHint;
	flags |= Qt::WindowCloseButtonHint;
	flags |= Qt::MSWindowsFixedSizeDialogHint;
	setWindowFlags(flags);

	QStringList headers;
	headers << tr("FileName") << tr("PictureName") << tr("Category");
	ui.itemList->setHorizontalHeaderLabels(headers);

	ui.itemList->horizontalHeader()->setSectionsClickable(false);
	ui.itemList->horizontalHeader()->setStyleSheet("font-weight:bold;");
	ui.itemList->horizontalHeader()->setFixedHeight(25);

	ui.itemList->setSelectionBehavior(QAbstractItemView::SelectRows);
	ui.itemList->setSelectionMode(QAbstractItemView::SingleSelection);
	ui.itemList->setColumnCount(3);
	
	connect(Setting::get(), SIGNAL(categoryAdded()), this, SLOT(reloadComboBox()));
	connect(Output::get(), SIGNAL(outputModified()), this, SLOT(reloadUIContent()));

	ui.modify->setEnabled(false);

	connect(ui.selectVideo, SIGNAL(clicked()), this, SLOT(onBrowseVideo()));
	connect(ui.selectPicture, SIGNAL(clicked()), this, SLOT(onBrowsePicture()));

	connect(ui.managerCategory, SIGNAL(clicked()), this, SLOT(onEditCategory()));

	connect(ui.add, SIGNAL(clicked()), this, SLOT(onAdd()));
	connect(ui.remove, SIGNAL(clicked()), this, SLOT(onRemove()));

	connect(ui.videoPath, SIGNAL(textChanged(const QString&)), this, SLOT(onVideoPathChanged(const QString&)));
	connect(ui.picturePath, SIGNAL(textChanged(const QString&)), this, SLOT(onPicturePathChanged(const QString&)));

	connect(ui.itemList, SIGNAL(cellDoubleClicked(int, int)), this, SLOT(onListItemDoubleClicked(int, int)));
	connect(ui.itemList, SIGNAL(currentItemChanged(QTableWidgetItem*, QTableWidgetItem*)), this, SLOT(onListItemSelectionChanged()));

	Setting::get()->read();
	Output::get()->read();

	reloadUIContent();
}

ConfigTools::~ConfigTools() {
}

void ConfigTools::reloadComboBox() {
	ui.categories->addItems(Setting::get()->videoCategories());
	ui.categories->setCurrentIndex(-1);
}

void ConfigTools::reloadUIContent() {
	ui.videoPath->clear();
	ui.picturePath->clear();

	ui.categories->clear();

	reloadComboBox();

	ui.itemList->clearContents();
	ui.itemList->setRowCount(0);

	auto cont = Output::get()->items();
	for (int i = 0; i < cont.size(); ++i) {
		appendRow(cont[i]);
	}

	ui.itemList->horizontalHeader()->resizeSections(QHeaderView::ResizeToContents);
	ui.itemList->horizontalHeader()->setStretchLastSection(true);
}

void ConfigTools::appendRow(const OutputItem& item) {
	int p = ui.itemList->rowCount();
	ui.itemList->insertRow(p);
	ui.itemList->setRowHeight(p, 22);

	QFileInfo vi(item.video);
	QFileInfo pi(item.picture);
	QTableWidgetItem* item0 = new QTableWidgetItem(vi.fileName());
	QTableWidgetItem* item1 = new QTableWidgetItem(pi.fileName());
	QTableWidgetItem* item2 = new QTableWidgetItem(item.category);

	ui.itemList->setItem(p, 0, item0);
	ui.itemList->setItem(p, 1, item1);
	ui.itemList->setItem(p, 2, item2);
}

void ConfigTools::onListItemSelectionChanged() {
	qDebug() << ui.itemList->selectedItems().size();
}

void ConfigTools::onListItemDoubleClicked(int row, int column) {
	if (!saved_) {
		auto ans = QMessageBox::information(this, tr("SavePromptTitle"), tr("SavePromptMessage"), MESSAGE_BUTTON_YES_NO | QMessageBox::Cancel);
		if (ans == QMessageBox::Cancel) {
			return;
		}

		if (ans == QMessageBox::Yes) {
			onAdd();
		}
	}
	
	const OutputItem& item = Output::get()->items()[row];
	ui.videoPath->setText(item.video);
	ui.picturePath->setText(item.picture);
	int index = Setting::get()->videoCategories().indexOf(item.category);
	ui.categories->setCurrentIndex(qMax(index, 0));
}

void ConfigTools::onRemove() {
	QList<QTableWidgetItem*> items = ui.itemList->selectedItems();
	if (items.isEmpty()) {
		QMessageBox::warning(this, tr("WarningTitle"), tr("NoRemoveItemPrompt"), QMessageBox::Ok);
		return;
	}

	auto ans = QMessageBox::warning(this, tr("WarningTitle"), tr("RemoveItemPrompt"), MESSAGE_BUTTON_YES_NO);
	if (ans == QMessageBox::No) {
		return;
	}

	int row = items.front()->row();
	
	ui.itemList->removeRow(row);
	Output::get()->remove(row);
}

void ConfigTools::onAdd() {
	QString videoPath = ui.videoPath->text();
	if (videoPath.isEmpty() || !QFile(videoPath).exists()) {
		QMessageBox::warning(this, tr("WarningTitle"), tr("InvalidVideoPathMessage"), QMessageBox::Ok);
		return;
	}

	QString picturePath = ui.picturePath->text();
	if (picturePath.isEmpty() || !QFile(picturePath).exists()) {
		QMessageBox::warning(this, tr("WarningTitle"), tr("InvalidPicturePathMessage"), QMessageBox::Ok);
		return;
	}

	int index = ui.categories->currentIndex();
	if (index < 0) {
		QMessageBox::warning(this, tr("WarningTitle"), tr("InvalidCategorySelectedMessage"), QMessageBox::Ok);
		return;
	}

	QString category;
	QStringList list = Setting::get()->videoCategories();
	if (index >= 0 && index < list.size()) {
		category = list[index];
	}

	OutputItem item = { videoPath.replace('\\', '/'), picturePath.replace('\\', '/'), category };
	if (Output::get()->add(item)) {
		appendRow(item);
		ui.videoPath->clear();
		ui.picturePath->clear();
		ui.categories->setCurrentIndex(0);
	}
}

void ConfigTools::onBrowseVideo() {
	QString file = QFileDialog::getOpenFileName(this, tr("SelectVideo"), "", tr("Videos") + " (" + Setting::get()->videoPrefix() + ")");
	if (file.isEmpty()) { return; }

	ui.videoPath->setText(file);
}

void ConfigTools::onEditCategory() {
	CategoryEditor* editor = new CategoryEditor(this);
	editor->exec();
}

void ConfigTools::onBrowsePicture() {
	QString file = QFileDialog::getOpenFileName(this, tr("SelectVideo"), "", tr("Videos") + " (" + Setting::get()->picturePrefix() + ")");
	if (file.isEmpty()) { return; }
	ui.picturePath->setText(file);
}

void ConfigTools::onVideoPathChanged(const QString& path) {
	QFile file(path);
	QString ff = "font-family: Microsoft Yahei; font-size:13px;";
	ui.videoPath->setStyleSheet(ff + (path.isEmpty() || file.exists() ? "color:black;" : "color:red;"));
}

void ConfigTools::onPicturePathChanged(const QString& path) {
	QFile file(path);
	QString ff = "font-family: Microsoft Yahei; font-size:13px;";
	ui.picturePath->setStyleSheet(ff + (path.isEmpty() || file.exists() ? "color:black;" : "color:red;"));
}
