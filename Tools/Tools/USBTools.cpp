#include <windows.h>
#include <winbase.h>  
#include <winnls.h>  
#include <dbt.h>

#include <QDebug>
#include "USBTools.h"

USBTools::USBTools() {
}

USBTools::~USBTools() {
}

void USBTools::scan() {
	char diskPath[5] = { 0 };
	DWORD allDisk = GetLogicalDrives(); //����һ��32λ����������ת���ɶ����ƺ󣬱�ʾ����,���λΪA��  
	if (allDisk == 0) { return; }

	for (int i = 0; i < 13; i++)     //�ٶ������12�����̴�A��ʼ����  
	{
		if ((allDisk & 1) != 1) { continue; }
		sprintf(diskPath, "%c", 'A' + i);
		strcat(diskPath, ":");
		LPCWSTR rootPath = (LPCWSTR)QString(diskPath).utf16();

		if (GetDriveType(rootPath) == DRIVE_REMOVABLE) {
			if (GetVolumeInformation(rootPath, 0, 0, 0, 0, 0, 0, 0)) //�ж������Ƿ�׼������  
			{
				qDebug() << "U��׼��������" << diskPath;
				QString  path(diskPath);
				addDisk(path);
			}
		}

		allDisk = allDisk >> 1;
	}
}

void USBTools::addDisk(QString& disk) {
	int iter = 0;
	if (!searchDisk(disk, iter)) {
		disks_.push_back(disk);
		emit onDiskAdded(disk);
	}
}

void USBTools::deleteDisk(QString& disk) {
	int iter = 0;
	if (searchDisk(disk, iter)) {
		disks_.remove(iter);
		emit onDiskRemoved(disk);
	}
}

bool USBTools::searchDisk(QString& disk, int& iter) {
	for (int i = 0; i < this->disks_.size(); i++) {
		if (disks_.at(i).compare(disk) == 0) {
			iter = i;
			return true;
		}
	}

	return false;
}

bool USBTools::onWinEvent(MSG* msg, long *result) {
	QString dirPath;
	int msgType = msg->message;
	if (msgType == WM_DEVICECHANGE) {
		qDebug() << "winEvent in MgFrame : WM_DEVICECHANGE";
		PDEV_BROADCAST_HDR lpdb = (PDEV_BROADCAST_HDR)msg->lParam;
		switch (msg->wParam) {
		case DBT_DEVICEARRIVAL:
			if (lpdb->dbch_devicetype == DBT_DEVTYP_VOLUME) {
				PDEV_BROADCAST_VOLUME lpdbv = (PDEV_BROADCAST_VOLUME)lpdb;
				if (lpdbv->dbcv_flags == 0) {
					//����U�̣���ȡ�̷�  
					dirPath = QString(firstDriveFromMask(lpdbv->dbcv_unitmask));
					dirPath += ":";
					addDisk(dirPath);
					qDebug() << "this->USBDisk Path =" << dirPath;
				}
			}
			break;
		case DBT_DEVICEREMOVECOMPLETE:
			if (lpdb->dbch_devicetype == DBT_DEVTYP_VOLUME) {
				PDEV_BROADCAST_VOLUME lpdbv = (PDEV_BROADCAST_VOLUME)lpdb;
				if (lpdbv->dbcv_flags == 0) {
					qDebug() << "USB_Removed";

					dirPath = QString(firstDriveFromMask(lpdbv->dbcv_unitmask));
					dirPath += ":";
					deleteDisk(dirPath);
					qDebug() << "this-delete is =" << dirPath;
				}
			}
			break;
		}
	}

	return false;
}

char USBTools::firstDriveFromMask(unsigned long mask) {
	char i;
	for (i = 0; i < 26; ++i, mask >>= 1) {
		if (mask & 0x1) { break; }
	}

	return (i + 'A');
}

