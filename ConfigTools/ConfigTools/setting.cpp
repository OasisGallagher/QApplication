#include <QFile>
#include <QString>
#include <QTextStream>
#include <QMessageBox>

#include "setting.h"
#include "defines.h"

#define tr(_Msg)	QObject::tr(_Msg)

bool Setting::open() {
	QFile file(SETTING_PATH);
	if (!file.exists()) {
		revert();
	}

	if (!file.open(QIODevice::ReadOnly)) {
		QMessageBox::warning(NULL, tr("FailedToOpenSettingTitle"), tr("FailedToOpenSettingMessage"));
		return false;
	}

	QDomDocument doc;
	if (!doc.setContent(&file, false, NULL, NULL, NULL) || !unserialize(&doc)) {
		QMessageBox::StandardButton ans = QMessageBox::warning(
			NULL, 
			tr("InvalidSettingFormatTitle"), 
			QString(tr("InvalidSettingFormatMessage")),
			QMessageBox::Yes | QMessageBox::No
		);

		if (ans != QMessageBox::Yes) {
			file.close();
			return false;
		}

		revert();
		unserialize(&doc);
	}

	file.close();

	return true;
}

bool Setting::unserialize(QDomDocument* doc) {
	QDomElement root = doc->documentElement().toElement();
	if (root.isNull()) { return false; }

	QDomNodeList categories = root.elementsByTagName(SETTING_CATEGORY);
	QDomElement vp = root.firstChildElement(SETTING_VIDEO_POSTFIX).toElement();
	QDomElement pp = root.firstChildElement(SETTING_PICTURE_POSTFIX).toElement();

	if (vp.isNull() || pp.isNull()) {
		return false;
	}

	categories_.clear();

	for (int i = 0; i < categories.size(); ++i) {
		categories_ << categories.at(i).firstChild().nodeValue();
	}

	videoPrefix_ = vp.firstChild().nodeValue();
	picturePrefix_ = pp.firstChild().nodeValue();

	return true;
}

void Setting::revert() {
	videoPrefix_ = "*.mp4 *.flv";
	picturePrefix_ = "*.jpg *.jpeg *.png";

	//categories_.clear();
	categories_ << "first" << "second";

	serialize();
}

bool Setting::addCategory(const QString& category) {
	if(!categories_.indexOf(category)) {
		categories_ << category;
		serialize();
		return true;
	}

	return false;
}

void Setting::removeCategory(const QString& category) {
	if (categories_.removeAll(category) != 0) {
		serialize();
	}
}

void Setting::serialize() {
	QFile file(SETTING_PATH);

	file.open(QIODevice::WriteOnly | QIODevice::Truncate);

	QTextStream stream(&file);
	QDomDocument doc;

	QDomProcessingInstruction instruction = doc.createProcessingInstruction("xml", "version = \'1.0\' encoding=\'UTF-8\'");
	doc.appendChild(instruction);

	QDomElement root = doc.createElement(SETTING_ROOT);
	doc.appendChild(root);

	QDomElement categoriesElement = doc.createElement(SETTING_CATEGORIES);
	root.appendChild(categoriesElement);

	foreach(QString str, categories_) {
		QDomElement e = doc.createElement(SETTING_CATEGORY);
		e.appendChild(doc.createTextNode(str));
		categoriesElement.appendChild(e);
	}

	QDomElement vp = doc.createElement(SETTING_VIDEO_POSTFIX);
	root.appendChild(vp);
	vp.appendChild(doc.createTextNode(videoPrefix_));

	QDomElement pp = doc.createElement(SETTING_PICTURE_POSTFIX);
	root.appendChild(pp);
	pp.appendChild(doc.createTextNode(picturePrefix_));

	doc.save(stream, 4);

	file.close();
}
