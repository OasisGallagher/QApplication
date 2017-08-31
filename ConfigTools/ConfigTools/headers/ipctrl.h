#pragma once
#include <QFrame>
class QLineEdit;

class IPCtrl : public QFrame {
	Q_OBJECT

public:
	IPCtrl(QWidget *parent = 0);
	~IPCtrl() {}

public:
	QString text() const;
	void clear();
	void setText(const QString& ip);

protected:
	virtual bool eventFilter(QObject *obj, QEvent *event);

public slots:
	void slotTextChanged(QLineEdit* pEdit);

signals:
	void signalTextChanged(QLineEdit* pEdit);

private:
	enum {
		QTUTL_IP_SIZE = 4,
		MAX_DIGITS = 3
	};

	QLineEdit *(m_pLineEdit[QTUTL_IP_SIZE]);
	void moveToNextField(int i);
	void moveToPrevField(int i);
};
