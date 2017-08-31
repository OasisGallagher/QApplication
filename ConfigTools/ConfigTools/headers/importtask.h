#pragma once
#include <QThread>

class ImportTask : public QThread {
	Q_OBJECT

public:
	ImportTask(QObject* parent);
	~ImportTask();
	void copy(const QStringList& videos, const QStringList& pictures, bool keepHierarchy);

protected:
	virtual void run();

private:
	void copyAllFiles();
	void cloneHierarchy();

	void cloneHierarchyTo(const QStringList& from, const QString& to);
	void copyFile(const QString& from, const QString& to, int& overrideChoise);

signals:
	void importFinished();
	void currentImporting(QString name);
	void overridePrompt(QString path);

private:
	QString to_;
	QStringList videos_;
	QStringList pictures_;
	bool keepHierarchy_;
};
