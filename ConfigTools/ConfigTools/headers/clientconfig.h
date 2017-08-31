#pragma once

#include <QObject>
#include <QVector>
#include <QByteArray>

#include "config.h"

struct ClientItem {
	ClientItem() {}
	ClientItem(QJsonObject jo);
	ClientItem(const QString& ip);

	QJsonObject toJson() const;
	bool operator == (const ClientItem& other) const;

	QString ip;
};

class ClientConfig : public QObject, public Config<ClientConfig, ClientItem> {
	Q_OBJECT

protected:
	virtual QString rootName();
	virtual QString configPath();
	virtual void configModified();

	template <class T, class Item> friend class Config;

signals:
	void clientConfigModified();

private:
	ClientConfig() {}
};
