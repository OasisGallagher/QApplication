#include <QMenu>
#include <QDebug>
#include <QMessageBox>

#include "defines.h"
#include "output.h"
#include "setting.h"
#include "categoryeditor.h"
#include "ui_categoryeditor.h"

CategoryEditor::CategoryEditor(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::CategoryEditor) {
	ui->setupUi(this);	

	connect(ui->itemList, SIGNAL(itemChanged(QListWidgetItem*)), this, SLOT(onItemChanged(QListWidgetItem*)));
	connect(ui->itemList, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(onItemListCustomContextMenuRequested(const QPoint&)));
}

CategoryEditor::~CategoryEditor() {
	delete ui;
}

void CategoryEditor::onItemListCustomContextMenuRequested(const QPoint&) {
	QList<QListWidgetItem*> list = ui->itemList->selectedItems();

	QMenu* menu = new QMenu(this);
	QAction* add = new QAction(tr("Add"), menu);
	QAction* remove = new QAction(tr("Remove"), menu);
	QAction* modify = new QAction(tr("Modify"), menu);

	connect(add, SIGNAL(triggered()), this, SLOT(onAddItem()));
	connect(modify, SIGNAL(triggered()), this, SLOT(onModifyItem()));
	connect(remove, SIGNAL(triggered()), this, SLOT(onRemoveItems()));

	if (list.isEmpty()) {
		remove->setEnabled(false);
		modify->setEnabled(false);
	}

	menu->addAction(add);
	menu->addAction(remove);
	menu->addAction(modify);
	
	menu->exec(QCursor::pos());
}

void CategoryEditor::onAddItem() {
	QListWidgetItem* item = new QListWidgetItem("");
	item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsEditable | Qt::ItemIsSelectable);

	ui->itemList->addItem(item);
	ui->itemList->editItem(item);
}

void CategoryEditor::onRemoveItems() {
	QStringList list;
	foreach(QListWidgetItem* item, ui->itemList->selectedItems()) {
		list << item->text();
	}

	if (Output::get()->countOfCategoriesUsed(list) > 0) {
		auto ans = QMessageBox::warning(this, tr("Warning"), "RemoveCategoryDependencyMessage", MESSAGE_BUTTON_YES_NO);
		if (ans == QMessageBox::No) {
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

void CategoryEditor::onModifyItem() {
	ui->itemList->editItem(ui->itemList->selectedItems().front());
}

void CategoryEditor::showEvent(QShowEvent* e) {
	const QStringList& categories = Setting::get()->videoCategories();
	ui->itemList->clear();

	foreach(QString str, categories) {
		QListWidgetItem* item = new QListWidgetItem(str);
		item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsEditable | Qt::ItemIsSelectable);
		ui->itemList->addItem(item);
	}
}

void CategoryEditor::onItemChanged(QListWidgetItem* item) {
	int row = ui->itemList->row(item);
	QStringList list = Setting::get()->videoCategories();
	if (row >= list.size()) {
		if (item->text().isEmpty()) {
			QMessageBox::warning(this, tr("Warning"), tr("EmptyCategoryMessage"), QMessageBox::Ok);
			ui->itemList->editItem(item);
		}
		else {
			Setting::get()->addCategory(item->text());
		}
	}
	else {
		QString old = Setting::get()->videoCategories()[row];
		Setting::get()->replaceCategory(old, item->text());
	}
}
