#include <functional>

#include <QDebug>
#include <QList>
#include <QMenu>
#include <QListView>
#include <QKeyEvent>
#include <QMessageBox>
#include <QToolButton>
#include <QFileDialog>
#include <QTreeWidgetItem>
#include <QFileIconProvider>

#include "videoconfig.h"
#include "clientconfig.h"

#include "importtask.h"
#include "importprogress.h"

#include "utility.h"
#include "setting.h"
#include "defines.h"
#include "usbtools.h"
#include "configtools.h"
#include "videoeditor.h"
#include "clienteditor.h"
#include "categoryeditor.h"
#include "overrideprompt.h"

#define MAGIC_VIDEO		'v'
#define MAGIC_PICTURE	'p'

ConfigTools::ConfigTools(QWidget *parent)
	: QDialog(parent) {
	ui.setupUi(this);
	
	currentDisk_ = new USBDisk;

	model_ = new QStandardItemModel(ui.udiskFileTree);
	ui.udiskFileTree->setModel(model_);

	videoEditor_ = new VideoEditor(this);
	clientEditor_ = new ClientEditor(this);
	categoryEditor_ = new CategoryEditor(this);
	usbTools_ = new USBTools(this);

	task_ = new ImportTask(this);
	progress_ = new ImportProgress(this);

	initializeUI();
	connectSignals();

	Setting::get()->read();
	VideoConfig::get()->read();
	ClientConfig::get()->read();

	reloadVideos();
	reloadClients();

	usbTools_->scan();
}

ConfigTools::~ConfigTools() {
	delete currentDisk_;
}

void ConfigTools::keyReleaseEvent(QKeyEvent* e) {
	if (e->key() == Qt::Key_Delete) {
		onRemoveVideo();
	}
}

bool ConfigTools::nativeEvent(const QByteArray &eventType, void *message, long *result) {
	usbTools_->onWinEvent((MSG*)message, result);
	return false;
}

void ConfigTools::initializeUI() {
	Qt::WindowFlags flags = windowFlags();
	flags |= Qt::WindowMinimizeButtonHint;
	flags |= Qt::WindowMaximizeButtonHint;
	flags |= Qt::WindowCloseButtonHint;
	//flags |= Qt::MSWindowsFixedSizeDialogHint;
	setWindowFlags(flags);

	model_->setColumnCount(2);
	QStringList fileTreeHeaders;
	fileTreeHeaders << tr("FileName") << tr("FileSize");
	model_->setHorizontalHeaderLabels(fileTreeHeaders);
	ui.udiskFileTree->header()->setStyleSheet("font-weight:bold;");

	ui.videoList->horizontalHeader()->setVisible(true);
	ui.videoList->horizontalHeader()->setStyleSheet("font-weight:bold;");
	ui.clientList->horizontalHeader()->setStyleSheet("font-weight:bold;");

	QStringList videoHeader;
	videoHeader << tr("FaceName") << tr("FileName") << tr("PictureName") << tr("Category");
	ui.videoList->setHorizontalHeaderLabels(videoHeader);

	QStringList clientHeader;
	clientHeader << tr("ClientIpAddress");
	ui.clientList->setHorizontalHeaderLabels(clientHeader);
	
	ui.tabWidget->removeTab(2);
}

void ConfigTools::connectSignals() {
	connect(task_, SIGNAL(importFinished()), this, SLOT(onImportFinished()));
	connect(task_, SIGNAL(overridePrompt(QString)), this, SLOT(onOverridePrompt(QString)));
	connect(task_, SIGNAL(currentImporting(QString)), this, SLOT(onCurrentImportingChanged(QString)));

	connect(ui.reloadDisk, SIGNAL(clicked()), this, SLOT(reloadFileTree()));
	connect(ui.importFiles, SIGNAL(clicked()), this, SLOT(importFiles()));

	connect(model_, SIGNAL(itemChanged(QStandardItem*)), this, SLOT(onTreeItemChanged(QStandardItem*)));

	connect(usbTools_, SIGNAL(diskAdded(const USBDisk&)), this, SLOT(onDiskAdded(const USBDisk&)));
	connect(usbTools_, SIGNAL(diskRemoved(const USBDisk&)), this, SLOT(onDiskRemoved(const USBDisk&)));

	connect(VideoConfig::get(), SIGNAL(videoConfigModified()), this, SLOT(reloadVideos()));
	connect(ui.editCategory, SIGNAL(clicked()), this, SLOT(onEditCategory()));

	connect(ui.addVideo, SIGNAL(clicked()), this, SLOT(onAddVideo()));
	connect(ui.modifyVideo, SIGNAL(clicked()), this, SLOT(onModifyVideo()));
	connect(ui.removeVideo, SIGNAL(clicked()), this, SLOT(onRemoveVideo()));

	connect(ui.videoList, SIGNAL(cellDoubleClicked(int, int)), this, SLOT(onVideoItemDoubleClicked(int, int)));
	connect(ui.videoList, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(onVideoTableCustomContextMenuRequested()));

	connect(ui.clientList, SIGNAL(cellDoubleClicked(int, int)), this, SLOT(onClientItemDoubleClicked(int, int)));
	connect(ui.clientList, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(onClientTableCustomContextMenuRequested()));

	connect(ui.addClient, SIGNAL(clicked()), this, SLOT(onAddClient()));
	connect(ui.modifyClient, SIGNAL(clicked()), this, SLOT(onModifyClient()));
	connect(ui.removeClient, SIGNAL(clicked()), this, SLOT(onRemoveClient()));
}

void ConfigTools::onDiskAdded(const USBDisk& disk) {
	if (currentDisk_->root.isEmpty()) {
		*currentDisk_ = disk;
		ui.tabWidget->addTab(ui.tab_3, tr("TabImportFromDisk"));
		ui.tabWidget->setTabIcon(2, QIcon(":/images/udisk"));
		reloadFileTree();
	}
}

void ConfigTools::onDiskRemoved(const USBDisk& disk) {
	if (currentDisk_->root == disk.root) {
		model_->setRowCount(0);
		ui.tabWidget->removeTab(2);
		currentDisk_->root.clear();
	}
}

void ConfigTools::reloadVideos() {
	ui.videoList->clearContents();
	ui.videoList->setRowCount(0);

	QStringList header;
	auto cont = VideoConfig::get()->items();
	for (int i = 0; i < cont.size(); ++i) {
		appendVideoRow(cont[i]);
		header << Utility::padding(i + 1);
	}

	ui.videoList->setVerticalHeaderLabels(header);
}

void ConfigTools::reloadClients() {
	ui.clientList->clearContents();
	ui.clientList->setRowCount(0);

	QStringList header;
	auto cont = ClientConfig::get()->items();
	for (int i = 0; i < cont.size(); ++i) {
		appendClientRow(cont[i]);
		header << Utility::padding(i + 1);
	}

	ui.clientList->setVerticalHeaderLabels(header);
}

void ConfigTools::appendVideoRow(const VideoItem& item) {
	int p = ui.videoList->rowCount();
	ui.videoList->insertRow(p);
	ui.videoList->setRowHeight(p, 22);

	QDir base(VIDEO_PATH);
	QFileInfo vi(item.video);
	QFileInfo pi(item.picture);
	QTableWidgetItem* item0 = new QTableWidgetItem(item.name);
	QTableWidgetItem* item1 = new QTableWidgetItem(vi.filePath());
	if (!base.exists(item.video)) {
		item1->setTextColor(QColor(255, 0, 0));
	}

	base.setPath(PICTURE_PATH);
	QTableWidgetItem* item2 = new QTableWidgetItem(pi.filePath());
	if (!base.exists(item.picture)) {
		item2->setTextColor(QColor(255, 0, 0));
	}

	QTableWidgetItem* item3 = new QTableWidgetItem(item.category);
	if (!Setting::get()->categories().contains(item.category)) {
		item3->setTextColor(QColor(255, 0, 0));
	}
	
	ui.videoList->setItem(p, 0, item0);
	ui.videoList->setItem(p, 1, item1);
	ui.videoList->setItem(p, 2, item2);
	ui.videoList->setItem(p, 3, item3);

	ui.videoList->resizeColumnsToContents();
}

void ConfigTools::appendClientRow(const ClientItem& item) {
	int p = ui.clientList->rowCount();
	ui.clientList->insertRow(p);
	ui.clientList->setRowHeight(p, 22);
	QTableWidgetItem* item0 = new QTableWidgetItem(item.ip);
	item0->setTextAlignment(Qt::AlignHCenter);
	ui.clientList->setItem(p, 0, item0);
}

bool ConfigTools::selectedTableRows(QTableWidget* table, QVector<int>* cont) {
	QList<QTableWidgetSelectionRange> ranges = table->selectedRanges();
	if (cont == NULL) { return !ranges.empty(); }

	foreach(QTableWidgetSelectionRange r, ranges) {	
		for (int i = r.topRow(); i <= r.bottomRow(); ++i) {
			*cont << i;
		}
	}

	qSort(cont->begin(), cont->end(), std::greater<int>());

	return !cont->empty();
}

void ConfigTools::onVideoTableCustomContextMenuRequested() {
	QMenu menu;
	QAction* add = new QAction(tr("Add"), &menu);
	QAction* remove = new QAction(tr("Remove"), &menu);
	QAction* modify = new QAction(tr("Modify"), &menu);

	connect(add, SIGNAL(triggered()), this, SLOT(onAddVideo()));
	connect(modify, SIGNAL(triggered()), this, SLOT(onModifyVideo()));
	connect(remove, SIGNAL(triggered()), this, SLOT(onRemoveVideo()));

	if (!selectedTableRows(ui.videoList, NULL)) {
		remove->setEnabled(false);
		modify->setEnabled(false);
	}

	menu.addAction(add);
	menu.addAction(modify);
	menu.addAction(remove);

	menu.exec(QCursor::pos());
}

void ConfigTools::onClientTableCustomContextMenuRequested() {
	QMenu menu;
	QAction* add = new QAction(tr("Add"), &menu);
	QAction* remove = new QAction(tr("Remove"), &menu);
	QAction* modify = new QAction(tr("Modify"), &menu);

	connect(add, SIGNAL(triggered()), this, SLOT(onAddClient()));
	connect(modify, SIGNAL(triggered()), this, SLOT(onModifyClient()));
	connect(remove, SIGNAL(triggered()), this, SLOT(onRemoveClient()));

	if (!selectedTableRows(ui.clientList, NULL)) {
		remove->setEnabled(false);
		modify->setEnabled(false);
	}

	menu.addAction(add);
	menu.addAction(modify);
	menu.addAction(remove);

	menu.exec(QCursor::pos());
}

void ConfigTools::onVideoItemDoubleClicked(int row, int column) {
	onModifyVideo();
}

void ConfigTools::onClientItemDoubleClicked(int row, int column) {
	onModifyClient();
}

void ConfigTools::onAddVideo() {
	if (Setting::get()->categories().isEmpty()) {
		Message::warning(this, tr("EmptyCategory"));
		return;
	}

	videoEditor_->exec(-1);
	if (videoEditor_->result() == QDialog::Accepted) {
		reloadVideos();
	}
}

void ConfigTools::onModifyVideo() {
	QVector<int> rows;

	if (!selectedTableRows(ui.videoList, &rows)) {
		Message::warning(this, tr("EmptySelectionMessage"));
		return;
	}

	videoEditor_->exec(rows.front());
	if (videoEditor_->result() == QDialog::Accepted) {
		reloadVideos();
	}
}

void ConfigTools::onRemoveVideo() {
	QList<QTableWidgetSelectionRange> ranges = ui.videoList->selectedRanges();
	int count = ranges.count();
	for (int i = 0; i < count; i++) {
		int topRow = ranges.at(i).topRow();
		int bottomRow = ranges.at(i).bottomRow();
	}

	QVector<int> rows;

	if (!selectedTableRows(ui.videoList, &rows)) {
		Message::warning(this, tr("NoRemoveItemPrompt"));
		return;
	}

	if (!Message::question(this, tr("RemoveItemPrompt").arg(rows.size()))) {
		return;
	}

	foreach (int r, rows) {
		ui.videoList->removeRow(r);
		VideoConfig::get()->remove(r);
	}
}

void ConfigTools::onAddClient() {
	clientEditor_->exec(-1);
	if (clientEditor_->result() == QDialog::Accepted) {
		reloadClients();
	}
}

void ConfigTools::onModifyClient() {
	QVector<int> rows;
	if (!selectedTableRows(ui.clientList, &rows)) {
		Message::warning(this, tr("EmptySelectionMessage"));
		return;
	}

	clientEditor_->exec(rows.front());
	if (clientEditor_->result() == QDialog::Accepted) {
		reloadVideos();
	}
}

void ConfigTools::onRemoveClient() {
	QVector<int> rows;
	if (!selectedTableRows(ui.clientList, &rows)) {
		Message::warning(this, tr("NoRemoveItemPrompt"));
		return;
	}

	if (!Message::question(this, tr("RemoveItemPrompt").arg(rows.size()))) {
		return;
	}

	foreach (int r, rows) {
		ui.clientList->removeRow(r);
		ClientConfig::get()->remove(r);
	}
}

void ConfigTools::onEditCategory() {
	categoryEditor_->exec();
}

void ConfigTools::onImportFinished() {
	progress_->hide();
}

void ConfigTools::onOverridePrompt(QString path) {
	OverridePrompt* prompt = new OverridePrompt(this);
	bool rememberMyChoise = false;
	int choice = prompt->exec(path, rememberMyChoise);
	
	Wait::value = Utility::makeDword(choice, (int)rememberMyChoise);
	Wait::lock();
	Wait::wake();
	Wait::unlock();
	delete prompt;
}

void ConfigTools::onCurrentImportingChanged(QString name) {
	progress_->setText(tr("CurrentImporting") + " " + name + "...");
}

void ConfigTools::onTreeItemChanged(QStandardItem* item) {
	if (item == Q_NULLPTR) { return; }
	if (item->isCheckable()) {
		Qt::CheckState state = item->checkState();
		if (item->isTristate()) {
			if (state != Qt::PartiallyChecked) {
				checkAllChild(item, state == Qt::Checked);
			}
		}
		else {
			onChildCheckStateChanged(item);
		}
	}
}

void ConfigTools::checkAllChild(QStandardItem * item, bool check) {
	int rowCount = item->rowCount();
	for (int i = 0; i < rowCount; ++i) {
		QStandardItem* childItems = item->child(i);
		checkAllChild(childItems, check);
	}

	if (item->isCheckable()) {
		item->setCheckState(check ? Qt::Checked : Qt::Unchecked);
	}
}

void ConfigTools::onChildCheckStateChanged(QStandardItem * item) {
	Qt::CheckState siblingState = checkSibling(item);
	QStandardItem * parentItem = item->parent();
	if (NULL == parentItem) { return; }

	if (Qt::PartiallyChecked == siblingState) {
		if (parentItem->isCheckable() && parentItem->isTristate()) {
			parentItem->setCheckState(Qt::PartiallyChecked);
		}
	}
	else if (Qt::Checked == siblingState) {
		if (parentItem->isCheckable()) {
			parentItem->setCheckState(Qt::Checked);
		}
	}
	else {
		if (parentItem->isCheckable()) {
			parentItem->setCheckState(Qt::Unchecked);
		}
	}

	onChildCheckStateChanged(parentItem);
}

Qt::CheckState ConfigTools::checkSibling(QStandardItem * item) {
	QStandardItem * parent = item->parent();
	if (nullptr == parent) {
		return item->checkState();
	}

	int brotherCount = parent->rowCount();
	int checkedCount = 0, unCheckedCount = 0;

	Qt::CheckState state;
	for (int i = 0; i < brotherCount; ++i) {
		QStandardItem* siblingItem = parent->child(i);
		state = siblingItem->checkState();
		if (Qt::PartiallyChecked == state) {
			return Qt::PartiallyChecked;
		}

		if (Qt::Unchecked == state) {
			++unCheckedCount;
		}
		else {
			++checkedCount;
		}

		if (checkedCount > 0 && unCheckedCount > 0) {
			return Qt::PartiallyChecked;
		}
	}

	return (unCheckedCount > 0) ? Qt::Unchecked : Qt::Checked;
}

void ConfigTools::importFiles() {
	if (currentDisk_->root.isEmpty()) {
		Message::warning(this, tr("NoUSBDiskFoundMessage"));
		return;
	}

	QStringList videos, pictures;
	if (!collectSelected(videos, pictures)) {
		Message::warning(this, tr("EmptySelectionMessage"));
		return;
	}

	task_->copy(videos, pictures, ui.keepHierarchy->isChecked());
	progress_->exec();
}

bool ConfigTools::collectSelected(QStringList& videos, QStringList& pictures) {
	QStandardItem* root = model_->item(0, 0);
	if (root == NULL) { return false; }

	collectSelectedRecursively(root, videos, pictures);

	return !videos.empty() || !pictures.empty();
}

void ConfigTools::collectSelectedRecursively(QStandardItem* root, QStringList& videos, QStringList& pictures) {
	int rows = root->rowCount();
	int column = root->columnCount();
	for (int i = 0; i < rows; ++i) {
		for (int j = 0; j < column; ++j) {
			QStandardItem* child = root->child(i, j);
			if (child == NULL) { continue; }
			QString path = child->data().toString();
			if (child->checkState() == Qt::Unchecked) { continue; }

			if (path.isEmpty()) {
				collectSelectedRecursively(child, videos, pictures);
				continue;
			}

			if (path.startsWith(MAGIC_VIDEO)) {
				videos.append(path.right(path.size() - 1));
			}
			else {
				pictures.append(path.right(path.size() - 1));
			}
		}
	}
}

void ConfigTools::reloadFileTree() {
	model_->setRowCount(0);
	QFileIconProvider ip;
	QFileInfo fileInfo(currentDisk_->root);
	QStandardItem* dirItem = new QStandardItem(ip.icon(fileInfo), QString("%1 (%2)").arg(currentDisk_->name).arg(currentDisk_->root));
	model_->appendRow(dirItem);
	dirItem->setTristate(true);
	dirItem->setCheckable(true);
	fillFileTreeRecursively(currentDisk_->root, dirItem);

	ui.udiskFileTree->expandAll();
	ui.udiskFileTree->header()->resizeSections(QHeaderView::ResizeToContents);
}

int ConfigTools::fillFileTreeRecursively(const QString& path, QStandardItem *parent) {
	QDir dir(path);
	if (!dir.exists()) { return false; }

	
	QFileInfoList directories = dir.entryInfoList(QDir::Dirs | QDir::NoSymLinks | QDir::NoDotAndDotDot);
	QFileInfoList videos = dir.entryInfoList(QStringList(Setting::get()->videoPrefix().split(' ')), QDir::Files);
	QFileInfoList pictures = dir.entryInfoList(QStringList(Setting::get()->picturePrefix().split(' ')), QDir::Files);

	QFileInfoList list;
	list << videos << pictures << directories;

	int videoCount = videos.size();
	int fileCount = videos.size() + pictures.size();
	
	for (int i = 0; i < list.size(); i++) {
		QFileInfo info = list.at(i);
		QFileIconProvider ip;

		if (info.isDir()) {
			QStandardItem* dirItem = new QStandardItem(ip.icon(info), info.fileName());
			
			int subFileCount = fillFileTreeRecursively(info.filePath(), dirItem);
			if (subFileCount == 0) {
				delete dirItem;
			}
			else {
				dirItem->setCheckable(true);
				dirItem->setTristate(true);
				parent->appendRow(dirItem);
			}

			fileCount += subFileCount;
		}
		else {
			QStandardItem *fileItem = new QStandardItem(ip.icon(info), info.fileName());
			fileItem->setCheckable(true);
			fileItem->setTristate(false);
			fileItem->setData((i < videoCount ? MAGIC_VIDEO : MAGIC_PICTURE) + info.filePath());

			parent->appendRow(fileItem);
			parent->setChild(fileItem->index().row(), 1, new QStandardItem(Utility::encodeSize(info.size())));
		}
	}

	return fileCount;
}
