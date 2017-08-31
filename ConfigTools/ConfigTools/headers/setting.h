#pragma once
#include <QObject>
#include <QStringList>
#include <QtXml/QDomDocument>

class Setting : public QObject {
	Q_OBJECT

public:
	static Setting* get();

	bool read();

	const QString categoryAt(int i) const { return categories_[i]; }
	const QStringList& categories() const { return categories_; }
	const QString& videoPrefix() const { return videoPrefix_; }
	const QString& picturePrefix() const { return picturePrefix_; }

	bool addCategory(const QString& category);
	void removeCategories(const QStringList& categories);
	bool replaceCategory(const QString& from, const QString& to);

signals:
	void categoryAdded();
	void categoriesRemoved(const QStringList& categories);
	void categoryModified(const QString& from, const QString& to);

private:
	Setting();

	void revert();
	void serialize();
	bool unserialize(QDomDocument* doc);

private:
	static Setting* ptr_;

	QString videoPrefix_;
	QString picturePrefix_;
	QStringList categories_;
};
