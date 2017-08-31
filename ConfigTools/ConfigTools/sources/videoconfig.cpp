#include "utility.h"
#include "setting.h"
#include "defines.h"
#include "videoconfig.h"

VideoConfig::VideoConfig() {
	connect(Setting::get(), SIGNAL(categoriesRemoved(const QStringList&)), this, SLOT(onCategoriesRemoved(const QStringList&)));
	connect(Setting::get(), SIGNAL(categoryModified(const QString&, const QString&)), this, SLOT(onCategoryModified(const QString&, const QString&)));
}

int VideoConfig::countOfCategoriesUsed(const QStringList& categories) {
	int ans = 0;
	for (int i = 0; i < cont_.size(); ++i) {
		if (categories.contains(cont_[i].category)) {
			++ans;
		}
	}

	return ans;
}

void VideoConfig::configModified() {
	emit videoConfigModified();
}

QString VideoConfig::rootName() {
	return VF_TAG_ROOT;
}

QString VideoConfig::configPath() {
	return VF_PATH;
}

void VideoConfig::onCategoriesRemoved(const QStringList& categories) {
	bool modified = true;

	for (int i = cont_.size() - 1; i >= 0 ; --i) {
		if (categories.contains(cont_[i].category)) {
			cont_.removeAt(i);
			modified = true;
		}
	}

	if (modified) {
		flush();
		emit videoConfigModified();
	}
}

void VideoConfig::onCategoryModified(const QString& from, const QString& to) {
	bool modifed = false;
	for (int i = 0; i < cont_.size(); ++i) {
		if (cont_[i].category == from) {
			cont_[i].category = to;
			modifed = true;
		}
	}

	if (modifed) {
		flush();
		emit videoConfigModified();
	}
}

VideoItem::VideoItem(const QString& name, const QString& video, const QString& picture, const QString& category) 
	: name(name), video(video), picture(picture), category(category) {
}

VideoItem::VideoItem(QJsonObject jo) {
	name = jo[VF_TAG_NAME].toString();
	video = jo[VF_TAG_VIDEO].toString();
	picture = jo[VF_TAG_PICTURE].toString();
	category = jo[VF_TAG_CATEGORY].toString();
}

QJsonObject VideoItem::toJson() const {
	QJsonObject jo;
	jo.insert(VF_TAG_NAME, name);
	jo.insert(VF_TAG_VIDEO, video);
	jo.insert(VF_TAG_PICTURE, picture);
	jo.insert(VF_TAG_CATEGORY, category);

	return jo;
}

bool VideoItem::operator==(const VideoItem& other) const {
	return video == other.video;
}
