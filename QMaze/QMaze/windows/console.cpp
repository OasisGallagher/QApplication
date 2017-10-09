#include "console.h"
#include <QHeaderView>

Console::Console(QWidget* parent) : QWidget(parent) {
}

void Console::initialize() {
	view_ = findChild<QTableWidget*>("view", Qt::FindDirectChildrenOnly);
	view_->horizontalHeader()->setStretchLastSection(true);
	view_->setColumnCount(2);
}

void Console::addMessage(MessageType type, const QString& message) {
	// TODO: log tags/filters.
	if (messages_.contains(QString::number(type) + message)) {
		return;
	}

	messages_.insert(QString::number(type) + message);
	int r = view_->rowCount();
	view_->insertRow(r);
	view_->setRowHeight(r, 20);
	view_->setColumnWidth(0, 24);

	QTableWidgetItem* icon = new QTableWidgetItem(QIcon(messageIconPath(type)), "");
	QTableWidgetItem* text = new QTableWidgetItem(message);
	view_->setItem(r, 0, icon);
	view_->setItem(r, 1, text);
}

const char* Console::messageIconPath(MessageType type) {
	const char* path = "";
	switch (type) {
		case Debug:
			path = ":/images/debug";
			break;
		case Warning:
			path = ":/images/warning";
			break;
		case Error:
			path = ":/images/error";
			break;
	}

	return path;
}