#pragma once

#include <QLineEdit>

class IPAddressField : public QLineEdit {
	Q_OBJECT
public:
	IPAddressField(QWidget *parent);
	~IPAddressField();
};

class IPAddressControl : public QLineEdit {
	Q_OBJECT
public:
	IPAddressControl(QWidget *parent);
	~IPAddressControl();
	void setGeometry(int x, int y, int w, int h);
	bool SetIPText(int nIP, int nIndex);
	int GetIPText(int nIndex);
private:
	void paintEvent(QPaintEvent *e);

private:
	IPAddressField *m_pLineEdit[4];
};
