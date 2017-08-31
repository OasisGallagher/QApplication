#include <QMenu>
#include <QDebug>
#include <QMessageBox>

#include "defines.h"
#include "videoconfig.h"
#include "setting.h"
#include "categoryeditor.h"
#include "ui_categoryeditor.h"

CategoryEditor::CategoryEditor(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::CategoryEditor) {
	ui->setupUi(this);	

	connect(ui->itemList, SIGNAL(itemChanged(QListWidgetItem*)), this, SLOT(onItemChanged(QListWidgetItem*)));
	connect(ui->itemList, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(onItemListCustomContextMenuRequested()));

	connect(ui->add, SIGNAL(clicked()), this, SLOT(onAdd()));
	connect(ui->modify, SIGNAL(clicked()), this, SLOT(onModify()));
	connect(ui->remove, SIGNAL(clicked()), this, SLOT(onRemove()));
}

CategoryEditor::~CategoryEditor() {
	delete ui;
}

void CategoryEditor::onItemListCustomContextMenuRequested() {
	QList<QListWidgetItem*> list = ui->itemList->selectedItems();

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
	menu.addAction(modify);
	menu.addAction(remove);

	menu.exec(QCursor::pos());
}

void CategoryEditor::onAdd() {
	QString name = newCategoryName();
	QListWidgetItem* item = new QListWidgetItem(name);
	item->setData(Qt::UserRole, name);
	Setting::get()->addCategory(item->text());

	item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsEditable | Qt::ItemIsSelectable);
	ui->itemList->addItem(item);
	ui->itemList->editItem(item);
	ui->itemList->clearSelection();
	item->setSelected(true);
}

QString CategoryEditor::newCategoryName() {
	auto items = Setting::get()->categories();
	QString ans = tr("NewCategory");
	for (int i = 1; items.contains(ans); ++i) {
		ans = QString(tr("NewCategory") + "%1").arg(i);
	}

	return ans;
}

void CategoryEditor::onRemove() {
	QStringList list;
	QList<QListWidgetItem*> selected = ui->itemList->selectedItems();
	foreach(QListWidgetItem* item, selected) {
		list << item->text();
	}

	if (VideoConfig::get()->countOfCategoriesUsed(list) > 0) {
		if (!Message::question(this, tr("RemoveCategoryDependencyMessage"))) {
			return;
		}
	}
	else {
		if (!Message::question(this, tr("RemoveItemPrompt").arg(selected.size()))) {
			return;
		}
	}

	foreach(QListWidgetItem* item, ui->itemList->selectedItems()) {
		for (int i = 0; i < ui->itemList->count(); ++i) {
			if (ui->itemList->item(i) == item) {
				QListWidgetItem* garbage = ui->itemList->takeItem(i);
				delete garbage;
			}
		}
	}

	Setting::get()->removeCategories(list);
}

void CategoryEditor::onModify() {
	QList<QListWidgetItem*> items = ui->itemList->selectedItems();
	if (items.isEmpty()) {
		Message::warning(this, tr("EmptySelectionMessage"));
		return;
	}

	ui->itemList->editItem(items.front());
}

void CategoryEditor::showEvent(QShowEvent* e) {
	const QStringList& categories = Setting::get()->categories();
	ui->itemList->clear();

	foreach(QString str, categories) {
		QListWidgetItem* item = new QListWidgetItem(str);
		item->setData(Qt::UserRole, str);
		item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsEditable | Qt::ItemIsSelectable);
		ui->itemList->addItem(item);
	}
}

void CategoryEditor::onItemChanged(QListWidgetItem* item) {
	int row = ui->itemList->row(item);
	QStringList list = Setting::get()->categories();
	bool done = false;

	if (item->text().isEmpty()) {
		Message::warning(this, tr("EmptyCategoryMessage"));
	}
	else if (row >= list.size()) {
		done = Setting::get()->addCategory(item->text());
	}
	else {
		QString old = Setting::get()->categoryAt(row);
		done = Setting::get()->replaceCategory(old, item->text());
	}

	if (done) {
		item->setData(Qt::UserRole, item->text());
	}
	else {
		item->setText(item->data(Qt::UserRole).toString());
		ui->itemList->clearSelection();
		item->setSelected(true);
	}
}
