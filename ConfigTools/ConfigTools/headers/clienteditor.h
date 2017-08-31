#pragma once

#include <QDialog>

namespace Ui {
class ClientEditor;
}

class ClientItem;

class ClientEditor : public QDialog {
	Q_OBJECT

public:
	explicit ClientEditor(QWidget *parent = 0);
	~ClientEditor();

public:
	void exec(int modifying);

private slots:
	void onAdd();

private:
	void addItem(const ClientItem& item);
	void modifyItem(const ClientItem& to);

private:
	int modifying_;
	Ui::ClientEditor *ui;
};
