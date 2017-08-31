#include <QDialog>

namespace Ui {
class OverridePrompt;
}

class OverridePrompt : public QDialog {
	Q_OBJECT

public:
	enum {
		Invalid = -1,
		CopyAndReplace,
		DonotCopy,
		CopyAndKeep2Files,
	};

public:
	explicit OverridePrompt(QWidget *parent = 0);
	~OverridePrompt();

public:
	int exec(const QString& path, bool& rememberMyChoise);

protected:
	virtual void closeEvent(QCloseEvent* e);

private slots:
	void onCommand();
	void openDirectory(QString path);

private:
	Ui::OverridePrompt *ui;
};
