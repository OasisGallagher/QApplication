#include <QFile>
#include <QFileDialog>

#include <QJsonArray>
#include <QJsonObject>
#include <QMessageBox>
#include <QJsonDocument>

#include "VideosConfig.h"
#include "setting.h"
#include "defines.h"

#define tr(_Msg)	QObject::tr(_Msg)

VideosConfig* VideosConfig::ptr_ = NULL;

VideosConfig* VideosConfig::get() {
	if (ptr_ == NULL) {
		ptr_ = new VideosConfig;
	}

	return ptr_;
}

VideosConfig::VideosConfig() {
	connect(Setting::get(), SIGNAL(categoriesRemoved(const QStringList&)), this, SLOT(onCategoriesRemoved(const QStringList&)));
	connect(Setting::get(), SIGNAL(categoryModified(const QString&, const QString&)), this, SLOT(onCategoryModified(const QString&, const QString&)));
}

bool VideosConfig::read() {
	QFile file(OUTPUT_PATH);
	if (!file.open(QIODevice::ReadOnly)) {
		return false;
	}

	QJsonParseError je;
	QJsonDocument jd = QJsonDocument::fromJson(file.readAll(), &je);

	if (je.error != QJsonParseError::NoError) {
		QMessageBox::StandardButton ans = QMessageBox::warning(NULL, tr("WarningTitle"), tr("InvalidJsonFormatMessage"), MESSAGE_BUTTON_YES_NO);
		if (ans == QMessageBox::Yes) {
			QFile::remove(OUTPUT_PATH);
		}
		
		return false;
	}

	cont_.clear();

	QJsonArray ja = jd.array();
	for (int i = 0; i < ja.size(); ++i) {
		QJsonObject jo = ja[i].toObject();
		VideoItem item{ jo[OUTPUT_VIDEO].toString(), jo[OUTPUT_PICTURE].toString(), jo[OUTPUT_CATEGORY].toString() };
		cont_.append(item);
	}

	return true;
}

int VideosConfig::countOfCategoriesUsed(const QStringList& categories) {
	int ans = 0;
	for (int i = 0; i < cont_.size(); ++i) {
		if (categories.contains(cont_[i].category)) {
			++ans;
		}
	}

	return ans;
}

void VideosConfig::onCategoriesRemoved(const QStringList& categories) {
	bool modified = true;

	for (int i = cont_.size() - 1; i >= 0 ; --i) {
		if (categories.contains(cont_[i].category)) {
			cont_.removeAt(i);
			modified = true;
		}
	}

	if (modified) {
		flush();
		emit outputModified();
	}
}

void VideosConfig::onCategoryModified(const QString& from, const QString& to) {
	bool modifed = false;
	for (int i = 0; i < cont_.size(); ++i) {
		if (cont_[i].category == from) {
			cont_[i].category = to;
			modifed = true;
		}
	}

	if (modifed) {
		flush();
		emit outputModified();
	}
}

void VideosConfig::flush() {
	/*QString OUTPUT_PATH = QFileDialog::getSaveFileName(NULL, tr("SaveOutputTitle"), DEFAULT_FILE, "*.json");
	if (OUTPUT_PATH.isEmpty()) {
		return;
	}
	*/
	QFile file(OUTPUT_PATH);
	file.open(QIODevice::WriteOnly | QIODevice::Truncate);
	file.write(format());
	file.close();
}

bool VideosConfig::add(const VideoItem& item) {
	int i = indexOf(item.video);

	if (i >= 0) {
		auto ans = QMessageBox::warning(NULL, tr("Warning"), tr("DuplicateItemOverrideMessage"), MESSAGE_BUTTON_YES_NO);

		if (ans == QMessageBox::No) {
			return false;
		}

		if (ans == QMessageBox::Yes) {
			cont_[i] = item;
		}
	}
	else {
		cont_.append(item);
	}

	flush();
	emit outputModified();

	return true;
}

void VideosConfig::remove(int i) {
	cont_.remove(i);
	flush();
	emit outputModified();
}

bool VideosConfig::modify(int i, const VideoItem& item) {
	// modify information.
	if (cont_[i].video == item.video) {
		cont_[i] = item;
	}
	else {
		int p = indexOf(item.video);
		if (p >= 0) {
			QMessageBox::information(NULL, tr("DuplicateItemTitle"), tr("DuplicateItemMessage"), QMessageBox::Yes);
			return false;
		}
	}

	cont_[i] = item;
	flush();
	emit outputModified();

	return true;
}

QByteArray VideosConfig::format() {
	QJsonArray ja;

	for (int i = 0; i < cont_.size(); ++i) {
		QJsonObject jo;
		jo.insert(OUTPUT_VIDEO, cont_[i].video);
		jo.insert(OUTPUT_PICTURE, cont_[i].picture);
		jo.insert(OUTPUT_CATEGORY, cont_[i].category);

		ja.append(jo);
	}

	QJsonDocument jd;
	jd.setArray(ja);

	return jd.toJson();
}

int VideosConfig::indexOf(const QString& video) {
	for (int i = 0; i < cont_.size(); ++i) {
		if (cont_[i].video == video) {
			return i;
		}
	}

	return -1;
}
