#include <QFile>
#include <QFileDialog>

#include <QJsonArray>
#include <QJsonObject>
#include <QMessageBox>
#include <QJsonDocument>

#include "output.h"
#include "defines.h"

#define tr(_Msg)	QObject::tr(_Msg)

bool Output::open() {
	QFile file(OUTPUT_PATH);
	if (!file.open(QIODevice::ReadOnly)) {
		return false;
	}

	QJsonParseError je;
	QJsonDocument jd = QJsonDocument::fromJson(file.readAll(), &je);

	if (je.error != QJsonParseError::NoError) {
		QMessageBox::StandardButton ans = QMessageBox::warning(NULL, tr("InvalidJsonFormatTitle"), tr("InvalidJsonFormatMessage"), QMessageBox::Yes | QMessageBox::No);
		if (ans == QMessageBox::Yes) {
			QFile::remove(OUTPUT_PATH);
		}

		return false;
	}

	cont_.clear();

	QJsonArray ja = jd.array();
	for (int i = 0; i < ja.size(); ++i) {
		QJsonObject jo = ja[i].toObject();
		OutputItem item{ jo[OUTPUT_VIDEO].toString(), jo[OUTPUT_PICTURE].toString() };
		cont_.append(item);
	}

	return true;
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

void Output::add(const OutputItem& item) {
	int i = 0;
	for (; i < cont_.size(); ++i) {
		if (cont_[i].video == item.video) {
			QMessageBox::StandardButton ans = QMessageBox::information(NULL, tr("DuplicateItemTitle"),
				tr("DuplicateItemMessage"), QMessageBox::Yes | QMessageBox::No);

			if (ans == QMessageBox::Yes) {
				cont_[i] = item;
			}

			break;
		}
	}

	if (i >= cont_.size()) {
		cont_.append(item);
	}
}

QByteArray Output::format() const {
	QJsonArray ja;

	for (int i = 0; i < cont_.size(); ++i) {
		QJsonObject jo;
		jo.insert(OUTPUT_VIDEO, cont_[i].video);
		jo.insert(OUTPUT_PICTURE, cont_[i].picture);

		ja.append(jo);
	}

	QJsonDocument jd;
	jd.setArray(ja);

	return jd.toJson();
}

void Output::clear() {
	cont_.clear();
}
