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
	DWORD allDisk = GetLogicalDrives(); //返回一个32位整数，将他转换成二进制后，表示磁盘,最低位为A盘  
	if (allDisk == 0) { return; }

	for (int i = 0; i < 13; allDisk >>= 1, i++) {
		if ((allDisk & 1) == 0) { continue; }
		QString root = QString("%1:\\").arg(char('A' + i));
		const wchar_t* wptr = (const wchar_t*)root.utf16();
		if (GetDriveType(wptr) == DRIVE_REMOVABLE && GetVolumeInformation(wptr, 0, 0, 0, 0, 0, 0, 0)) {
			addDisk(root);
		}
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
					//插入U盘，获取盘符  
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

