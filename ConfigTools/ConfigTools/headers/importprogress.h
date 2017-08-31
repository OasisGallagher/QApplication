#pragma once

#include <QDialog>

namespace Ui {
class ImportProgress;
}

class ImportProgress : public QDialog {
	Q_OBJECT

public:
	explicit ImportProgress(QWidget *parent = 0);
	~ImportProgress();

public:
	void setText(const QString& text);

private:
	Ui::ImportProgress *ui;
};
