#pragma once

#include <QObject>
#include <QVector>
#include <QByteArray>

struct VideoItem {
	VideoItem(){}

	VideoItem(const QString& v, const QString& p, const QString& c) {
		video = v;
		picture = p;
		category = c;
	}

	QString video;
	QString picture;
	QString category;
};

class VideosConfig : public QObject {
	Q_OBJECT

public:
	static VideosConfig* get();

	bool read();

	bool add(const VideoItem& item);
	void remove(int i);
	bool modify(int i, const VideoItem& item);
	const VideoItem& itemAt(int i) const { return cont_[i]; }
	const QVector<VideoItem>& items() const { return cont_; }
	int countOfCategoriesUsed(const QStringList& categories);

signals:
	void outputModified();

private slots:
	void onCategoriesRemoved(const QStringList& categories);
	void onCategoryModified(const QString& from, const QString& to);

private:
	VideosConfig();
	void flush();
	QByteArray format();
	int indexOf(const QString& video);

private:
	static VideosConfig* ptr_;
	QVector<VideoItem> cont_;
};
