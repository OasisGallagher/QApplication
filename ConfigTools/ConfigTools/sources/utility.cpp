#include <QDir>
#include <QMessageBox>

#include "utility.h"

QMutex Wait::mutex_;
QVariant Wait::value;
QWaitCondition Wait::condition_;

int Utility::loword(int dword) {
	return dword & 0xffff;
}

int Utility::highword(int dword) {
	return (dword >> 16) & 0xffff;
}

int Utility::makeDword(int low, int high) {
	return (low & 0xffff) | ((high & 0xffff) << 16);
}

QString Utility::encodeSize(qint64 size) {
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

QString Utility::alternativeFileName(const QString& path) {
	if (!QFile::exists(path)) { return path; }
	QFileInfo info(path);
	QDir dir(info.path());

	QString suffix = info.completeSuffix();
	if (!suffix.isEmpty()) { suffix.push_front('.'); }

	QString newName;
	for (int i = 1; ; ++i) {
		newName = QString("%1 (%2)%3").arg(info.baseName()).arg(i).arg(suffix);
		if (!dir.exists(newName)) {
			break;
		}
	}

	return newName;
}

QString Utility::alternativeFilePath(const QString& path) {
	return QFileInfo(path).dir().filePath(alternativeFileName(path));
}

QString Utility::padding(int x, int width) {
	return QString("%1").arg(x, width, 10, QLatin1Char('0'));
}

QString Utility::elidedText(QLabel* label, const QString& text) {
	QFontMetrics fontMetrics(label->font());
	int fontSize = fontMetrics.width(text);
	int w = label->width();
	if (fontSize > label->width()) {
		return fontMetrics.elidedText(text, Qt::ElideMiddle, label->width());
	}
	
	return text;
}

void Message::warning(QWidget* parent, const QString& text) {
	QMessageBox box(QMessageBox::Warning, QObject::tr("Warning"), text, QMessageBox::Ok, parent);
	box.setButtonText(QMessageBox::Ok, QObject::tr("Ok"));
	box.exec();
}

bool Message::question(QWidget* parent, const QString& text) {
	QMessageBox::StandardButtons buttons = (QMessageBox::Yes | QMessageBox::No);

	QMessageBox box(QMessageBox::Question, QObject::tr("Question"), text, buttons, parent);
	box.setButtonText(QMessageBox::Yes, QObject::tr("Yes"));
	box.setButtonText(QMessageBox::No, QObject::tr("No"));

	return box.exec() == (int)QMessageBox::Yes;
}
