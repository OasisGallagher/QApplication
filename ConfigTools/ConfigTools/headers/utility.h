#pragma once
#include <QLabel>
#include <QString>
#include <QStringList>
#include <QMessageBox>

#include <QMutex>
#include <QVariant>
#include <QWaitCondition>

class Utility {
public:
	static int makeDword(int low, int high);
	static int loword(int dword);
	static int highword(int dword);

	static QString encodeSize(qint64 size);
	static QString padding(int x, int width = 3);
	static QString alternativeFilePath(const QString& path);
	static QString alternativeFileName(const QString& path);
	static QString elidedText(QLabel* label, const QString& text);
};

class Message {
public:
	static void warning(QWidget* parent, const QString& text);
	static bool question(QWidget* parent, const QString& text);
};

class Wait {
public:
	static void lock() { mutex_.lock(); }
	static void wait() { condition_.wait(&mutex_); }

	static void unlock() { mutex_.unlock(); }
	static void wake() { condition_.wakeAll(); }

	static QVariant value;

private:
	static QMutex mutex_;
	static QWaitCondition condition_;
};
