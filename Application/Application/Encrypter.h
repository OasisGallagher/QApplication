#pragma once
#include <QThread>

class Algorithm;

class Encrypter : public QThread {
	Q_OBJECT

public:
	Encrypter(QObject* app);
	~Encrypter();

public:
	void encrypt(const QString& path);
	void interrupt();

	int progress() const;

signals:
	void finished(QString);

private:
	virtual void run();

private:
	QString path_;
	QString out_path_;

	QObject* app_;

	int progress_;

	qint64 file_size_;
	qint64 out_size_;

	Algorithm* algorithm_;
};
