#include <QFile>
#include <QFileDialog>

#include <QJsonArray>
#include <QJsonObject>
#include <QMessageBox>
#include <QJsonDocument>

#include "output.h"
#include "setting.h"
#include "defines.h"

#define tr(_Msg)	QObject::tr(_Msg)

Output* Output::ptr_ = NULL;

Output* Output::get() {
	if (ptr_ == NULL) {
		ptr_ = new Output;
	}

	return ptr_;
}

Output::Output() {
	connect(Setting::get(), SIGNAL(categoriesRemoved(const QStringList&)), this, SLOT(onCategoriesRemoved(const QStringList&)));
	connect(Setting::get(), SIGNAL(categoryModified(const QString&, const QString&)), this, SLOT(onCategoryModified(const QString&, const QString&)));
}

bool Output::read() {
	QFile file(OUTPUT_PATH);
	if (!file.open(QIODevice::ReadOnly)) {
		return false;
	}

	QJsonParseError je;
	QJsonDocument jd = QJsonDocument::fromJson(file.readAll(), &je);

	if (je.error != QJsonParseError::NoError) {
		QMessageBox::StandardButton ans = QMessageBox::warning(NULL, tr("WarningTitle"), tr("InvalidJsonFormatMessage"), MESSAGE_BUTTON_YES_NO);
		if (ans == QMessageBox::Yes) {
			QFile::remove(OUTPUT_PATH);
		}
		
		return false;
	}

	cont_.clear();

	QJsonArray ja = jd.array();
	for (int i = 0; i < ja.size(); ++i) {
		QJsonObject jo = ja[i].toObject();
		OutputItem item{ jo[OUTPUT_VIDEO].toString(), jo[OUTPUT_PICTURE].toString(), jo[OUTPUT_CATEGORY].toString() };
		cont_.append(item);
	}

	return true;
}

int Output::countOfCategoriesUsed(const QStringList& categories) {
	int ans = 0;
	for (int i = 0; i < cont_.size(); ++i) {
		if (categories.contains(cont_[i].category)) {
			++ans;
		}
	}

	return ans;
}

void Output::onCategoriesRemoved(const QStringList& categories) {
	bool modified = true;

	for (int i = cont_.size() - 1; i >= 0 ; --i) {
		if (categories.contains(cont_[i].category)) {
			cont_.removeAt(i);
			modified = true;
		}
	}

	if (modified) {
		flush();
		emit outputModified();
	}
}

void Output::onCategoryModified(const QString& from, const QString& to) {
	bool modifed = false;
	for (int i = 0; i < cont_.size(); ++i) {
		if (cont_[i].category == from) {
			cont_[i].category = to;
			modifed = true;
		}
	}

	if (modifed) {
		flush();
		emit outputModified();
	}
}

void Output::flush() {
	/*QString OUTPUT_PATH = QFileDialog::getSaveFileName(NULL, tr("SaveOutputTitle"), DEFAULT_FILE, "*.json");
	if (OUTPUT_PATH.isEmpty()) {
		return;
	}
	*/
	QFile file(OUTPUT_PATH);
	file.open(QIODevice::WriteOnly | QIODevice::Truncate);
	file.write(format());
	file.close();
}

bool Output::add(const OutputItem& item) {
	int i = 0;
	for (; i < cont_.size(); ++i) {
		if (cont_[i].video == item.video) {
			QMessageBox::StandardButton ans = QMessageBox::information(NULL, tr("DuplicateItemTitle"),
				tr("DuplicateItemMessage"), MESSAGE_BUTTON_YES_NO | QMessageBox::Cancel);

			if (ans == QMessageBox::Cancel) {
				return false;
			}

			if (ans == QMessageBox::Yes) {
				cont_[i] = item;
			}

			break;
		}
	}

	if (i >= cont_.size()) {
		cont_.append(item);
	}

	flush();

	return true;
}

void Output::remove(int i) {
	cont_.remove(i);
	flush();
}

QByteArray Output::format() {
	QJsonArray ja;

	for (int i = 0; i < cont_.size(); ++i) {
		QJsonObject jo;
		jo.insert(OUTPUT_VIDEO, cont_[i].video);
		jo.insert(OUTPUT_PICTURE, cont_[i].picture);
		jo.insert(OUTPUT_CATEGORY, cont_[i].category);

		ja.append(jo);
	}

	QJsonDocument jd;
	jd.setArray(ja);

	return jd.toJson();
}
