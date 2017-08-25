#ifndef CATEGORYEDITOR_H
#define CATEGORYEDITOR_H

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

protected:
	virtual void showEvent(QShowEvent* e);

private slots:
	void onItemChanged(QListWidgetItem* item);
	void onItemListCustomContextMenuRequested(const QPoint& pos);
	void onAddItem();
	void onRemoveItems();
	void onModifyItem();

private:
	Ui::CategoryEditor *ui;
};

#endif // CATEGORYEDITOR_H
