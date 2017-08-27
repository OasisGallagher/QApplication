#include <windows.h>
#include <winbase.h>  
#include <winnls.h>  
#include <dbt.h>

#include <QDebug>
#include "USBTools.h"

void USBTools::scan() {
	DWORD drivers = GetLogicalDrives();
	
	for (int i = 0; drivers != 0 && i < 13; drivers >>= 1, i++) {
		if ((drivers & 1) == 0) { continue; }
		QString root = QString("%1:\\").arg(char('A' + i));
		LPCWSTR wptr = (LPCWSTR)root.utf16();
		WCHAR name[MAX_PATH + 1];
		if (GetDriveType(wptr) == DRIVE_REMOVABLE) {
			addDisk(root);
		}
	}
}

void USBTools::addDisk(const QString& root) {
	if (findDisk(root) >= 0) { return; }
	WCHAR name[MAX_PATH + 1];
	if(!GetVolumeInformation((LPCWSTR)root.utf16(), name, _countof(name), NULL, NULL, NULL, NULL, NULL)) {
		//QMessageBox::
	}
	USBDisk disk = { root , QString::fromWCharArray(name) };
	disks_.push_back(disk);
	emit onDiskAdded(disk);
}

void USBTools::deleteDisk(const QString& root) {
	int pos = 0;
	if ((pos = findDisk(root)) >= 0) {
		emit onDiskRemoved(disks_[pos]);
		disks_.remove(pos);
	}
}

int USBTools::findDisk(const QString& root) const {
	for (int i = 0; i < disks_.size(); i++) {
		if (disks_[i].root == root) {
			return i;
		}
	}

	return -1;
}

bool USBTools::onWinEvent(MSG* msg, long *result) {
	int type = msg->message;
	if (type == WM_DEVICECHANGE) {
		PDEV_BROADCAST_HDR lpdb = (PDEV_BROADCAST_HDR)msg->lParam;
		switch (msg->wParam) {
		case DBT_DEVICEARRIVAL:
			if (lpdb->dbch_devicetype == DBT_DEVTYP_VOLUME) {
				PDEV_BROADCAST_VOLUME lpdbv = (PDEV_BROADCAST_VOLUME)lpdb;
				if (lpdbv->dbcv_flags == 0) {
					addDisk(QString(firstDriveFromMask(lpdbv->dbcv_unitmask)) + ":\\");
				}
			}
			break;
		case DBT_DEVICEREMOVECOMPLETE:
			if (lpdb->dbch_devicetype == DBT_DEVTYP_VOLUME) {
				PDEV_BROADCAST_VOLUME lpdbv = (PDEV_BROADCAST_VOLUME)lpdb;
				if (lpdbv->dbcv_flags == 0) {
					deleteDisk(QString(firstDriveFromMask(lpdbv->dbcv_unitmask)) + ":\\");
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

