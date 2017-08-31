#pragma once
#include <QFile>
#include <QVector>
#include <QObject>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonParseError>

#include "utility.h"
#include "defines.h"

template <class T, class Item>
class Config {
public:
	static T* get() {
		if (instance == NULL) { instance = new T; }
		return instance;
	}

public:
	bool read() {
		QFile file(configPath());
		if (!file.exists()) {
			revert();
		}

		if (!file.open(QIODevice::ReadOnly)) {
			Message::warning(NULL, QObject::tr("FailedToOpenConfigFile").arg(configPath()));
			return false;
		}

		QJsonParseError je;
		QJsonDocument jd = QJsonDocument::fromJson(file.readAll(), &je);

		if (je.error != QJsonParseError::NoError) {
			if (!Message::question(NULL, QObject::tr("InvalidJsonFormatMessage").arg(configPath()).arg(je.errorString()))) {
				return false;
			}

			revert();
		}

		cont_.clear();

		QJsonObject root = jd.object();
		QJsonArray ja = root[rootName()].toArray();
		
		for (int i = 0; i < ja.size(); ++i) {
			QJsonObject jo = ja[i].toObject();
			Item item(jo);
			cont_.append(item);
		}

		return true;
	}

	bool add(const Item& item) {
		int i = cont_.indexOf(item);

		if (i >= 0) {
			
			if (!Message::question(NULL, QObject::tr("DuplicateItemOverrideMessage"))) {
				return false;
			}

			cont_[i] = item;
		}
		else {
			cont_.append(item);
		}

		flush();
		configModified();

		return true;
	}

	void remove(int i) {
		cont_.remove(i);
		flush();
		configModified();
	}

	bool modify(int i, const Item& item) {
		// modify information.
		if (cont_[i] == item) {
			cont_[i] = item;
		}
		else {
			int p = cont_.indexOf(item);
			if (p >= 0) {
				Message::warning(NULL, QObject::tr("DuplicateItemMessage"));
				return false;
			}
		}

		cont_[i] = item;
		flush();
		configModified();

		return true;
	}

	void flush() {
		QFile file(configPath());
		file.open(QIODevice::WriteOnly | QIODevice::Truncate);
		file.write(format());
		file.close();
	}

	const Item& itemAt(int i) const { return cont_[i]; }
	const QVector<Item>& items() const { return cont_; }

protected:
	virtual QString rootName() = 0;
	virtual QString configPath() = 0;
	virtual void configModified() = 0;
	
private:
	void revert() {
		cont_.clear();
		flush();
	}

	QByteArray format() {
		QJsonArray ja;

		for (int i = 0; i < cont_.size(); ++i) {
			QJsonObject jo = cont_[i].toJson();
			jo.insert(TAG_ID, Utility::padding(i + 1));
			ja.append(jo);
		}

		QJsonObject root;
		root.insert(rootName(), ja);

		QJsonDocument jd;
		jd.setObject(root);

		return jd.toJson();
	}

protected:
	QVector<Item> cont_;

private:
	static T* instance;
};

template<class T, class Item> 
T* Config<T, Item>::instance = NULL;
