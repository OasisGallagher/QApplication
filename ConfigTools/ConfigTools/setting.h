#pragma once
#include <QStringList>

class Setting {
public:
	bool open();

	const QString& videoPrefix() const { return prefix_; }
	const QStringList& videoCategories() const { return categories_; }

	void addCategory(const QString& category);
	void removeCategory(const QString& category);

private:
	void revert();
	void revertFromMemory();

private:
	QString prefix_;
	QStringList categories_;
};
