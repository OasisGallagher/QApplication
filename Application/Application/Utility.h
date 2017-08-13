#pragma once
#include <QString>
#include <QStringList>

class Utility {
public:
	static QString encodeSize(qint64 size) {
		static char* measures[] = {
			"B", "K", "M", "G", "T", "P", "E", "Z", "Y"
		};
	
		const int count = sizeof(measures) / sizeof(measures[0]);
		int i = 0;

		double ds = size;
		for (; ds > 1024 && i < count; ++i) {
			ds /= 1024;
		}

		return QString("%1 %2").arg(ds, 0, 'f', 1).arg(measures[i]);
	}
};
