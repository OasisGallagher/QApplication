#pragma once

#include <QQueue>
#include <QMutex>
#include <QVector>
#include <QObjectUserData>
#include <QtWidgets/QDialog>

#include "Encrypter.h"
#include "ui_Application.h"

struct Task {
	QString path;
	Encrypter* encrypter;
};

enum Language {
	Chinese,
	English,
};

class Application : public QDialog {
	Q_OBJECT

public:
	Application(QWidget *parent = Q_NULLPTR);
	~Application();

public:
	bool addTask(const QString& path, Encrypter* encrypter);
	bool removeTask(const QString& path);

	void setProgress(const QString& path, int value);

private:
	void initWindowWidgets();

	int findTask(const QString& path);

	void encryptFiles(const QStringList& paths);
	QStringList enumFiles(const QString& path);

private slots:
	void onClickOpenFile();
	void onClickOpenFolder();
	void onThreadFinished(QString path);

	void timerEvent(QTimerEvent *event);
	void contextMenuEvent(QContextMenuEvent* event);
	void onToggleLanguage();

private:
	int timer_id_;
	Language language_;

	QMutex mutex_;
	QVector<Task> tasks_;
	QQueue<QString> queue_;

private:
	Ui::Application ui;
};
