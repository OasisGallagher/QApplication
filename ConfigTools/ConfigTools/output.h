#pragma once

#include <QObject>
#include <QVector>
#include <QByteArray>

struct OutputItem {
	QString video;
	QString picture;
	QString category;
};

class Output : public QObject {
	Q_OBJECT

public:
	static Output* get();

	bool read();

	bool add(const OutputItem& item);
	void remove(int i);
	const QVector<OutputItem>& items() { return cont_; }
	int countOfCategoriesUsed(const QStringList& categories);

signals:
	void outputModified();

private slots:
	void onCategoriesRemoved(const QStringList& categories);
	void onCategoryModified(const QString& from, const QString& to);

private:
	Output();
	void flush();
	QByteArray format();

private:
	static Output* ptr_;
	QVector<OutputItem> cont_;
};
