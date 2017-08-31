#pragma once

#include <QDialog>

namespace Ui {
class CategoryEditor;
}

class QListWidgetItem;

class CategoryEditor : public QDialog {
	Q_OBJECT

public:
	explicit CategoryEditor(QWidget *parent = 0);
	~CategoryEditor();

private slots:
	void onItemChanged(QListWidgetItem* item);
	void onItemListCustomContextMenuRequested();
	void onAdd();
	void onRemove();
	void onModify();

protected:
	virtual void showEvent(QShowEvent* e);

private:
	QString newCategoryName();

private:
	Ui::CategoryEditor *ui;
};
