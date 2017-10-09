#pragma once
#include <QWidget>
#include <QTableWidget>

class Console : public QWidget {
	Q_OBJECT

public:
	friend class QMaze;
	Console(QWidget* parent = Q_NULLPTR);

public:
	enum MessageType {
		Debug,
		Warning,
		Error,
	};

public:
	void addMessage(MessageType type, const QString& message);

private:
	void initialize();
	const char* messageIconPath(MessageType type);

private:
	QTableWidget* view_;
	QSet<QString> messages_;
};
