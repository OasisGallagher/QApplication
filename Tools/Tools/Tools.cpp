#include <QDebug>
#include <QMessageBox>

#include <QDirModel>

#include "Tools.h"
#include "USBTools.h"

#include <QStandardItemModel>

Tools::Tools(QWidget *parent)
	: QDialog(parent) {
	ui.setupUi(this);

	usb_ = new USBTools;
	usb_->scan();

	__testTree();
}

Tools::~Tools() {
	delete usb_;
}

bool Tools::__testShowFiles(QString path, QStandardItemModel* model, QStandardItem *parent) {
	QDir dir(path);
	if (!dir.exists()) { return false; }

	dir.setFilter(QDir::Dirs | QDir::Files | QDir::NoSymLinks | QDir::NoDotAndDotDot);
	QFileInfoList list = dir.entryInfoList();
	int size = list.size();
	for (int i = 0; i < size; i++) {
		QFileInfo info = list.at(i);

		if (info.isDir()) {
			QStandardItem* dirItem = new QStandardItem(info.fileName());
			dirItem->setCheckable(true);
			
			if (parent == Q_NULLPTR) {
				model->appendRow(dirItem);
			}
			else {
				parent->appendRow(dirItem);
				parent->setChild(dirItem->index().row(), 1, dirItem);
			}

			bool tristate = __testShowFiles(info.filePath(), model, dirItem);
			dirItem->setTristate(tristate);
		}
		else {
			QStandardItem *fileItem = new QStandardItem(info.fileName());
			fileItem->setCheckable(true);
			fileItem->setTristate(false);
			if (parent == Q_NULLPTR) {
				model->appendRow(fileItem);
			}
			else {
				parent->appendRow(fileItem);
				parent->setChild(fileItem->index().row(), 1, fileItem);
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

	__testShowFiles("C:/Users/liam.wang/Desktop/Player", model, nullptr);

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
	//关联项目属性改变的信号和槽
	connect(model, SIGNAL(itemChanged(QStandardItem*)), this, SLOT(treeItemChanged(QStandardItem*)));
	//connect(model,SIGNAL(itemChanged(QStandardItem*)),this,SLOT(treeItemChanged(QStandardItem*)));
	ui.fileTree->setModel(model);
}

void Tools::treeItemChanged(QStandardItem* item) {
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
