#include "defines.h"
#include "utility.h"
#include "clientconfig.h"

void ClientConfig::configModified() {
	emit clientConfigModified();
}

QString ClientConfig::rootName() {
	return CF_TAG_ROOT;
}

QString ClientConfig::configPath() {
	return CF_PATH;
}

ClientItem::ClientItem(const QString& ip) : ip(ip) {
}

ClientItem::ClientItem(QJsonObject jo) {
	ip = jo[CF_TAG_IP].toString();
}

QJsonObject ClientItem::toJson() const {
	QJsonObject jo;
	jo.insert(CF_TAG_IP, ip);
	return jo;
}

bool ClientItem::operator==(const ClientItem& other) const {
	return ip == other.ip;
}
