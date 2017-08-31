#include <QDir>
#include <QObject>
#include <QWaitCondition>

#include "defines.h"
#include "utility.h"
#include "importtask.h"
#include "overrideprompt.h"

ImportTask::ImportTask(QObject* parent) : QThread(parent) {
}

ImportTask::~ImportTask() {
}

void ImportTask::run() {
	if (!keepHierarchy_) {
		copyAllFiles();
	}
	else {
		cloneHierarchy();
	}

	emit importFinished();
}

void ImportTask::copy(const QStringList& videos, const QStringList& pictures, bool keepHierarchy) {
	videos_ = videos;
	pictures_ = pictures;
	keepHierarchy_ = keepHierarchy;
	start();
}

void ImportTask::copyFile(const QString& from, const QString& to, int& overrideChoise) {
	QFileInfo fromFileInfo(from);
	QString toFilePath = QDir(to).filePath(fromFileInfo.fileName());
	emit currentImporting(fromFileInfo.fileName());

	int currentChoise = OverridePrompt::Invalid;
	if (QFile::exists(toFilePath)) {
		if (overrideChoise == OverridePrompt::Invalid) {
			Wait::lock();
			emit overridePrompt(toFilePath);
			Wait::wait();
			Wait::unlock();

			int value = Wait::value.toInt();
			bool rememberMyChoise = Utility::highword(value) != 0;
			int choice = Utility::loword(value);

			if (rememberMyChoise) {
				overrideChoise = choice;
			}

			currentChoise = choice;
		}
		else {
			currentChoise = overrideChoise;
		}
	}

	switch (currentChoise) {
	case OverridePrompt::DonotCopy:
		break;
	case OverridePrompt::CopyAndReplace:
		QFile::remove(toFilePath);
	case OverridePrompt::Invalid:
		QFile::copy(from, toFilePath);
		break;
	case OverridePrompt::CopyAndKeep2Files:
		QFile::copy(from, Utility::alternativeFilePath(toFilePath));
		break;
	default:
		break;
	}
}

void ImportTask::copyAllFiles() {
	int overrideChoise = -1;
	foreach(QString str, videos_) {
		copyFile(str, VIDEO_PATH, overrideChoise);
	}

	overrideChoise = -1;
	foreach(QString str, pictures_) {
		copyFile(str, PICTURE_PATH, overrideChoise);
	}
}

void ImportTask::cloneHierarchy() {
	cloneHierarchyTo(videos_, VIDEO_PATH);
	cloneHierarchyTo(pictures_, PICTURE_PATH);
}

void ImportTask::cloneHierarchyTo(const QStringList& from, const QString& to) {
	QDir toDir(to);
	bool autoOverride = false;

	int overrideChoise = OverridePrompt::Invalid;

	foreach(QString str, from) {
		QString toPath = QFileInfo(str).path();
		// remove Driver:/
		toPath = toPath.right(toPath.length() - toPath.indexOf('/') - 1);

		toDir.mkpath(toPath);
		toPath = toDir.absoluteFilePath(toPath);

		copyFile(str, toPath, overrideChoise);
	}
}
