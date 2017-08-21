#pragma once
#include <QObject>
#include <QVector>
#include <QString>
#include <windows.h>

class USBTools : public QObject {
	Q_OBJECT
public:
	USBTools();
	~USBTools();

public:
	void scan();
	bool onWinEvent(MSG* msg, long* result);

signals:
	void onDiskAdded(QString disk);
	void onDiskRemoved(QString disk);

private:
	void addDisk(QString& disk);
	void deleteDisk(QString& disk);
	bool searchDisk(QString& disk, int& iter);
	char firstDriveFromMask(unsigned long unitmask);

private:
	QVector<QString> disks_;
	QString currentDisk_;
};
