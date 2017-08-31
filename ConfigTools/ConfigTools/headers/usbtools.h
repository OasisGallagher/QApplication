#pragma once
#include <QObject>
#include <QVector>
#include <QString>
#include <windows.h>

struct USBDisk {
	QString root;
	QString name;
};

class USBTools : public QObject {
	Q_OBJECT
public:
	USBTools(QObject* parent = Q_NULLPTR) : QObject(parent) {}
	~USBTools() {}

public:
	void scan();
	int diskCount() const;
	bool onWinEvent(MSG* msg, long* result);

signals:
	void diskAdded(const USBDisk& disk);
	void diskRemoved(const USBDisk& disk);

private:
	void addDisk(const QString& root);
	void deleteDisk(const QString& root);
	int findDisk(const QString& root) const;
	char firstDriveFromMask(unsigned long mask);

private:
	QVector<USBDisk> disks_;
};
