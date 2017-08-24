#pragma once
#include <QStringList>
#include <QtXml/QDomDocument>

class Setting {
public:
	bool open();

	const QString& videoPrefix() const { return videoPrefix_; }
	const QString& picturePrefix() const { return picturePrefix_; }
	const QStringList& videoCategories() const { return categories_; }

	bool addCategory(const QString& category);
	void removeCategory(const QString& category);

private:
	void revert();
	void serialize();
	bool unserialize(QDomDocument* doc);

private:
	QString videoPrefix_;
	QString picturePrefix_;
	QStringList categories_;
};
