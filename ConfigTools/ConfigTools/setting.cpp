#include <QFile>
#include <QString>
#include <QTextStream>
#include <QMessageBox>
#include <QtXml/QDomDocument>

#include "setting.h"

static const char* path = "data/setting.xml";
#define tr(_Msg)	QObject::tr(_Msg)

bool Setting::open() {
	QFile file(path);
	if (!file.exists()) {
		revert();
	}

	if (!file.open(QIODevice::ReadOnly)) {
		QMessageBox::warning(NULL, tr("FailedToOpenSettingTitle"), tr("FailedToOpenSettingMessage"));
		return false;
	}

	QDomDocument doc;
	QString err;
	int line = 0, column = 0;
	if (!doc.setContent(&file, false, &err, &line, &column)) {
		QMessageBox::warning(NULL, tr("InvalidSettingFormatTitle"), QString(tr("InvalidSettingFormatMessage")).arg(err).arg(line).arg(column), QMessageBox::Yes | QMessageBox::No);
		return false;
	}
}

void Setting::revert() {
	prefix_ = "*.mp4 *.flv";
	categories_.clear();

	revertFromMemory();
}

void Setting::revertFromMemory() {
	QFile file(path);
	Q_ASSERT(!file.exists());

	file.open(QIODevice::WriteOnly);

	QTextStream stream(&file);
	QDomDocument doc;

	QDomProcessingInstruction instruction = doc.createProcessingInstruction("xml", "version = \'1.0\' encoding=\'UTF-8\'");
	doc.appendChild(instruction);

	QDomElement root = doc.createElement("setting");
	doc.appendChild(root);

	QDomElement categoriesElement = doc.createElement("categories");
	doc.appendChild(categoriesElement);

	foreach(QString str, categories_) {
		QDomElement e = doc.createElement("category");
		e.appendChild(doc.createTextNode(str));
		categoriesElement.appendChild(e);
	}

	QDomElement videoPostfix = doc.createElement("videoPostfix");
	doc.appendChild(videoPostfix);
	videoPostfix.appendChild(doc.createTextNode(prefix_));

	doc.save(stream, 4);

	file.close();
}
