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
	USBTools() {}
	~USBTools() {}

public:
	void scan();
	bool onWinEvent(MSG* msg, long* result);

signals:
	void onDiskAdded(const USBDisk& disk);
	void onDiskRemoved(const USBDisk& disk);

private:
	void addDisk(const QString& root);
	void deleteDisk(const QString& root);
	int findDisk(const QString& root) const;
	char firstDriveFromMask(unsigned long mask);

private:
	QVector<USBDisk> disks_;
};
