#pragma once

#include <QVector>
#include <QByteArray>

struct OutputItem {
	QString video;
	QString picture;
};

class Output {
public:
	bool open();

	void add(const OutputItem& item);
	void clear();
	void flush();

	const QVector<OutputItem>& items() const { return cont_; }

private:
	QByteArray format() const;

private:
	QVector<OutputItem> cont_;
};
