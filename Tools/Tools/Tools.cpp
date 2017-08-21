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

	QStandardItem* itemProject = new QStandardItem(/*m_publicIconMap[QStringLiteral("treeItem_Project")], */QStringLiteral("��Ŀ"));
	model->appendRow(itemProject);
	model->setItem(model->indexFromItem(itemProject).row(), 1, new QStandardItem(QStringLiteral("��Ŀ��Ϣ˵��")));

	QStandardItem* itemFolder = new QStandardItem(/*m_publicIconMap[QStringLiteral("treeItem_folder")],*/ QStringLiteral("�ļ���1"));
	itemProject->appendRow(itemFolder);
	itemProject->setChild(itemFolder->index().row(), 1, new QStandardItem(QStringLiteral("��Ϣ˵��")));
	itemFolder = new QStandardItem(/*m_publicIconMap[QStringLiteral("treeItem_folder")],*/ QStringLiteral("�ļ���2"));
	itemProject->appendRow(itemFolder);
	for (int i = 0; i < 5; ++i) {
		QStandardItem* itemgroup = new QStandardItem(/*m_publicIconMap[QStringLiteral("treeItem_group")],*/ QStringLiteral("��%1").arg(i + 1));
		itemFolder->appendRow(itemgroup);
		for (int j = 0; j < (i + 1); ++j) {
			QStandardItem* itemchannel = new QStandardItem(/*m_publicIconMap[QStringLiteral("treeItem_channel")],*/ QStringLiteral("Ƶ��%1").arg(j + 1));
			itemgroup->appendRow(itemchannel);
			itemgroup->setChild(itemchannel->index().row(), 1, new QStandardItem(QStringLiteral("Ƶ��%1��Ϣ˵��").arg(j + 1)));
		}
	}
	itemProject->setChild(itemFolder->index().row(), 1, new QStandardItem(QStringLiteral("�ļ���2��Ϣ˵��")));
	itemProject = new QStandardItem(/*m_publicIconMap[QStringLiteral("treeItem_Project")],*/ QStringLiteral("��Ŀ2"));
	model->appendRow(itemProject);
	for (int i = 0; i < 3; ++i) {
		itemFolder = new QStandardItem(/*m_publicIconMap[QStringLiteral("treeItem_folder")],*/ QStringLiteral("��Ŀ2�ļ���%1").arg(i + 1));
		itemFolder->setCheckable(true);
		itemFolder->setTristate(true);
		QStandardItem* itemFolderDes = new QStandardItem(/*m_publicIconMap[QStringLiteral("treeItem_group")],*/ QStringLiteral("�ļ���%1��").arg(i + 1));
		itemProject->appendRow(itemFolder);
		itemProject->setChild(itemFolder->index().row(), 1, itemFolderDes);
		for (int j = 0; j < i + 1; ++j) {
			QStandardItem* item = new QStandardItem(/*m_publicIconMap[QStringLiteral("treeItem_dataItem")],*/ QStringLiteral("��Ŀ%1").arg(j + 1));
			item->setCheckable(true);
			itemFolder->appendRow(item);

		}
	}

#endif
	//������Ŀ���Ըı���źźͲ�
	connect(model, SIGNAL(itemChanged(QStandardItem*)), this, SLOT(treeItemChanged(QStandardItem*)));
	//connect(model,SIGNAL(itemChanged(QStandardItem*)),this,SLOT(treeItemChanged(QStandardItem*)));
	ui.fileTree->setModel(model);
}

void Tools::treeItemChanged(QStandardItem* item) {
	if (item == Q_NULLPTR) { return; }
	if (item->isCheckable()) {
		//�����Ŀ�Ǵ��ڸ�ѡ��ģ���ô�ͽ�������Ĳ���  
		Qt::CheckState state = item->checkState(); //��ȡ��ǰ��ѡ��״̬  
		if (item->isTristate()) {
			//�����Ŀ����̬�ģ�˵�����Զ���Ŀ¼����ȫѡ��ȫ��ѡ������  
			if (state != Qt::PartiallyChecked) {
				//��ǰ��ѡ��״̬����Ҫ��������Ŀ����ȫѡ  
				checkAllChild(item, state == Qt::Checked);
			}
		}
		else {
			//˵������̬�ģ���̬��Ը�������̬��Ӱ��  
			//�ж��ֵܽڵ�����  
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
	//��ͨ�����ڵ��ȡ�ֵܽڵ�  
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
