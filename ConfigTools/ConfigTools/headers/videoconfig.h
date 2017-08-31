#pragma once

#include "config.h"

struct VideoItem {
	VideoItem() {}
	VideoItem(QJsonObject jo);
	VideoItem(const QString& name, const QString& video, const QString& picture, const QString& category);

	QJsonObject toJson() const;
	bool operator == (const VideoItem& other) const;

	QString name;
	QString video;
	QString picture;
	QString category;
};

class VideoConfig : public QObject, public Config<VideoConfig, VideoItem> {
	Q_OBJECT

public:
	int countOfCategoriesUsed(const QStringList& categories);

protected:
	template <class T, class Item> friend class Config;

	virtual QString rootName();
	virtual QString configPath();
	virtual void configModified();

signals:
	void videoConfigModified();

private slots:
	void onCategoriesRemoved(const QStringList& categories);
	void onCategoryModified(const QString& from, const QString& to);

private:
	VideoConfig();
};
