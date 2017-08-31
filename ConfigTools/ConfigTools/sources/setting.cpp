#include <QDir>
#include <QFile>
#include <QString>
#include <QTextStream>

#include "setting.h"
#include "defines.h"
#include "utility.h"

Setting* Setting::ptr_ = NULL;

Setting::Setting() {
	QDir dir;
	if (!dir.exists(VIDEO_PATH)) { dir.mkdir(VIDEO_PATH); }
	if (!dir.exists(PICTURE_PATH)) { dir.mkdir(PICTURE_PATH); }
}

Setting* Setting::get() {
	if (ptr_ == NULL) {
		ptr_ = new Setting;
	}

	return ptr_;
}

bool Setting::read() {
	QFile file(SETTING_PATH);
	if (!file.exists()) {
		revert();
	}

	if (!file.open(QIODevice::ReadOnly)) {
		Message::warning(NULL, tr("FailedToOpenSettingMessage"));
		return false;
	}

	QDomDocument doc;
	if (!doc.setContent(&file, false, NULL, NULL, NULL) || !unserialize(&doc)) {
		if (!Message::question(NULL, tr("InvalidSettingFormatMessage"))) {
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
	videoPrefix_ = "*.mp4 *.flv *.mkv *.avi *.wmv *.rmvb";
	picturePrefix_ = "*.jpg *.jpeg *.png *.bmp";

	categories_.clear();
	//categories_ << "first" << "second";

	serialize();
}

bool Setting::addCategory(const QString& category) {
	if(!categories_.contains(category)) {
		categories_ << category;
		serialize();
		emit categoryAdded();
		return true;
	}

	Message::warning(NULL, tr("DuplicateCategoryMessage"));
	return false;
}

void Setting::removeCategories(const QStringList& categories) {
	foreach(QString item, categories) {
		categories_.removeAll(item);
	}

	serialize();
	categoriesRemoved(categories);
}

bool Setting::replaceCategory(const QString& from, const QString& to) {
	if (from == to) { return true; }

	if (categories_.contains(to)) {
		Message::warning(NULL, tr("DuplicateCategoryMessage"));
		return false;
	}

	bool modified = false;
	for (int i = 0; i < categories_.size(); ++i) {
		if (categories_[i] == from) {
			categories_[i] = to;
			modified = true;
			break;
		}
	}

	if (modified) {
		serialize();
		emit categoryModified(from, to);
	}

	return true;
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
