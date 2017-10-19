#pragma once
#include <QSet>
#include <QWidget>

class QTableWidget;
class QTextBrowser;

class Console : public QWidget {
	Q_OBJECT

public:
	friend class Suede;
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
