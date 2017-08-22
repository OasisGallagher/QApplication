#include <QMenu>
#include <QDebug>
#include <QDirModel>
#include <QTableView>
#include <QMessageBox>

#include <QtXml/QDomDocument>

#include "Tools.h"
#include "Utility.h"

void testXml(QString path) {
	QFile file(path);
	file.open(QIODevice::WriteOnly | QIODevice::Truncate);
	
	QTextStream out(&file);
	
	QDomDocument doc;
	QDomProcessingInstruction instruction = doc.createProcessingInstruction("xml", "version = \'1.0\' encoding=\'UTF-8\'");
	doc.appendChild(instruction);

	QDomElement root = doc.createElement("root");
	doc.appendChild(root);

	QDomElement element = doc.createElement("objects");
	QDomAttr attr = doc.createAttribute("name");
	attr.setValue("first");
	element.setAttributeNode(attr);

	attr = doc.createAttribute("path");
	attr.setValue(":/");
	element.setAttributeNode(attr);

	root.appendChild(element);
	
	doc.save(out, 4);

	QDomNodeList list = doc.elementsByTagName("objects");
	qDebug() << list.size();
}

Tools::Tools(QWidget *parent)
	: QDialog(parent) {
	ui.setupUi(this);

	testXml("C:/Users/liam.wang/Desktop/1.xml");

	icons_[IconFile] = QIcon(":/img/file");
	icons_[IconDirectory] = QIcon(":/img/dir");

	tree_model_ = new QStandardItemModel(this);
	ui.fileTree->setModel(tree_model_);
	
	QStringList headers;
	headers << tr("Column 1") << tr("Column 2");
	tree_model_->setHorizontalHeaderLabels(headers);

	connect(tree_model_, SIGNAL(itemChanged(QStandardItem*)), this, SLOT(onTreeItemChanged(QStandardItem*)));

	ui.videoList->setViewMode(QListView::IconMode);

	videos_model_ = new QStandardItemModel(this);

	ui.videoList->setModel(videos_model_);
	ui.videoTable->setModel(videos_model_);

	usb_ = new USBTools;
	connect(usb_, SIGNAL(onDiskAdded(const USBDisk&)), this, SLOT(onDiskAdded(const USBDisk&)));
	connect(usb_, SIGNAL(onDiskRemoved(const USBDisk&)), this, SLOT(onDiskRemoved(const USBDisk&)));

	usb_->scan();

	ui.videoList->setContextMenuPolicy(Qt::CustomContextMenu);
	ui.videoTable->setContextMenuPolicy(Qt::CustomContextMenu);
	connect(ui.videoList, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(onVideosContextMenu(const QPoint&)));
	connect(ui.videoTable, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(onVideosContextMenu(const QPoint&)));
}

Tools::~Tools() {
	delete usb_;
}

void Tools::onDiskAdded(const USBDisk& disk) {
	updateTree(disk);
	updateVideosModel(disk);
}

void Tools::onVideosContextMenu(const QPoint& point) {
	QMenu* menu = new QMenu(this);
	QAction* action = menu->addAction(ui.videos->currentIndex() == 0 ? tr("IconView") : tr("ListView"));

	connect(action, SIGNAL(triggered(bool)), this, SLOT(onToggleVideosView()));

	menu->exec(QCursor::pos());
}

void Tools::onToggleVideosView() {
	ui.videos->setCurrentIndex(1 - ui.videos->currentIndex());
}

void Tools::updateVideosModel(const USBDisk& disk) {
	QStringList headers;
	headers << tr("Column 1") << tr("Column 2");

	QDir dir(disk.root);

	dir.setFilter(QDir::Dirs | QDir::Files | QDir::NoSymLinks | QDir::NoDotAndDotDot);
	QFileInfoList list = dir.entryInfoList();
	int size = list.size();
	for (int i = 0; i < size; i++) {
		QFileInfo info = list.at(i);
		QFileIconProvider ip;

		if (info.isDir()) {
			QStandardItem* dirItem = new QStandardItem(ip.icon(info), info.fileName());
			dirItem->setCheckable(true);

			videos_model_->appendRow(dirItem);
		}
		else {
			QStandardItem *fileItem = new QStandardItem(ip.icon(info), info.fileName());
			fileItem->setCheckable(true);
			videos_model_->appendRow(fileItem);
		}
	}
}

void Tools::updateTree(const USBDisk& disk) {
	initItemModel(disk.root, tree_model_, nullptr);
}

void Tools::onDiskRemoved(const USBDisk& disk) {
	tree_model_->clear();
	videos_model_->clear();
}

bool Tools::initItemModel(QString path, QStandardItemModel* model, QStandardItem *parent) {
	QDir dir(path);
	if (!dir.exists()) { return false; }

	dir.setFilter(QDir::Dirs | QDir::Files | QDir::NoSymLinks | QDir::NoDotAndDotDot);
	QFileInfoList list = dir.entryInfoList();
	int size = list.size();
	for (int i = 0; i < size; i++) {
		QFileInfo info = list.at(i);
		QFileIconProvider ip;

		if (info.isDir()) {
			QStandardItem* dirItem = new QStandardItem(ip.icon(info), info.fileName());
			dirItem->setCheckable(true);
			
			if (parent == Q_NULLPTR) {
				model->appendRow(dirItem);
			}
			else {
				parent->appendRow(dirItem);
				parent->setChild(dirItem->index().row(), 1, dirItem);
			}

			bool tristate = initItemModel(info.filePath(), model, dirItem);
			dirItem->setTristate(tristate);
		}
		else {
			QStandardItem *fileItem = new QStandardItem(ip.icon(info), info.fileName());
			fileItem->setCheckable(true);
			fileItem->setTristate(false);
			if (parent == Q_NULLPTR) {
				model->appendRow(fileItem);
				model->setItem(model->indexFromItem(fileItem).row(), 1, new QStandardItem(Utility::encodeSize(info.size())));
			}
			else {
				parent->appendRow(fileItem);
				parent->setChild(fileItem->index().row(), 1, new QStandardItem(Utility::encodeSize(info.size())));
			}
		}
	}

	return (size > 1);
}

void Tools::__testTree() {
	
	QStandardItemModel* model = new QStandardItemModel(ui.fileTree);
	
	QStringList headers;
	headers << tr("Column 1") << tr("Column 2");
	model->setHorizontalHeaderLabels(headers);

	initItemModel("C:/Users/liam.wang/Desktop/Player", model, nullptr);

#if 0

	QStandardItem* itemProject = new QStandardItem(/*m_publicIconMap[QStringLiteral("treeItem_Project")], */QStringLiteral("项目"));
	model->appendRow(itemProject);
	model->setItem(model->indexFromItem(itemProject).row(), 1, new QStandardItem(QStringLiteral("项目信息说明")));

	QStandardItem* itemFolder = new QStandardItem(/*m_publicIconMap[QStringLiteral("treeItem_folder")],*/ QStringLiteral("文件夹1"));
	itemProject->appendRow(itemFolder);
	itemProject->setChild(itemFolder->index().row(), 1, new QStandardItem(QStringLiteral("信息说明")));
	itemFolder = new QStandardItem(/*m_publicIconMap[QStringLiteral("treeItem_folder")],*/ QStringLiteral("文件夹2"));
	itemProject->appendRow(itemFolder);
	for (int i = 0; i < 5; ++i) {
		QStandardItem* itemgroup = new QStandardItem(/*m_publicIconMap[QStringLiteral("treeItem_group")],*/ QStringLiteral("组%1").arg(i + 1));
		itemFolder->appendRow(itemgroup);
		for (int j = 0; j < (i + 1); ++j) {
			QStandardItem* itemchannel = new QStandardItem(/*m_publicIconMap[QStringLiteral("treeItem_channel")],*/ QStringLiteral("频道%1").arg(j + 1));
			itemgroup->appendRow(itemchannel);
			itemgroup->setChild(itemchannel->index().row(), 1, new QStandardItem(QStringLiteral("频道%1信息说明").arg(j + 1)));
		}
	}
	itemProject->setChild(itemFolder->index().row(), 1, new QStandardItem(QStringLiteral("文件夹2信息说明")));
	itemProject = new QStandardItem(/*m_publicIconMap[QStringLiteral("treeItem_Project")],*/ QStringLiteral("项目2"));
	model->appendRow(itemProject);
	for (int i = 0; i < 3; ++i) {
		itemFolder = new QStandardItem(/*m_publicIconMap[QStringLiteral("treeItem_folder")],*/ QStringLiteral("项目2文件夹%1").arg(i + 1));
		itemFolder->setCheckable(true);
		itemFolder->setTristate(true);
		QStandardItem* itemFolderDes = new QStandardItem(/*m_publicIconMap[QStringLiteral("treeItem_group")],*/ QStringLiteral("文件夹%1组").arg(i + 1));
		itemProject->appendRow(itemFolder);
		itemProject->setChild(itemFolder->index().row(), 1, itemFolderDes);
		for (int j = 0; j < i + 1; ++j) {
			QStandardItem* item = new QStandardItem(/*m_publicIconMap[QStringLiteral("treeItem_dataItem")],*/ QStringLiteral("项目%1").arg(j + 1));
			item->setCheckable(true);
			itemFolder->appendRow(item);

		}
	}

#endif
	connect(model, SIGNAL(itemChanged(QStandardItem*)), this, SLOT(onTreeItemChanged(QStandardItem*)));
	ui.fileTree->setModel(model);
}

void Tools::onTreeItemChanged(QStandardItem* item) {
	if (item == Q_NULLPTR) { return; }
	if (item->isCheckable()) {
		//如果条目是存在复选框的，那么就进行下面的操作  
		Qt::CheckState state = item->checkState(); //获取当前的选择状态  
		if (item->isTristate()) {
			//如果条目是三态的，说明可以对子目录进行全选和全不选的设置  
			if (state != Qt::PartiallyChecked) {
				//当前是选中状态，需要对其子项目进行全选  
				checkAllChild(item, state == Qt::Checked);
			}
		}
		else {
			//说明是两态的，两态会对父级的三态有影响  
			//判断兄弟节点的情况  
			onChildCheckStateChanged(item);
		}
	}
}

void Tools::checkAllChild(QStandardItem * item, bool check) {
	int rowCount = item->rowCount();
	for (int i = 0; i < rowCount; ++i) {
		QStandardItem* childItems = item->child(i);
		checkAllChild(childItems, check);
	}

	if (item->isCheckable()) {
		item->setCheckState(check ? Qt::Checked : Qt::Unchecked);
	}
}

void Tools::onChildCheckStateChanged(QStandardItem * item) {
	Qt::CheckState siblingState = checkSibling(item);
	QStandardItem * parentItem = item->parent();
	if (nullptr == parentItem) { return; }

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

Qt::CheckState Tools::checkSibling(QStandardItem * item) {
	//先通过父节点获取兄弟节点  
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

bool Tools::nativeEvent(const QByteArray &eventType, void *message, long *result) {
	Q_UNUSED(eventType);

	usb_->onWinEvent((MSG*)message, result);

	return false;
}
