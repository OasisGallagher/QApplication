#include <QDialog>

#include "ui_videoeditor.h"

struct VideoItem;

class VideoEditor : public QDialog {
	Q_OBJECT
public:
	explicit VideoEditor(QWidget *parent = 0);

public:
	void exec(int modifying = -1);

private slots:
	void onAdd();
	void onBrowseVideo();
	void onBrowsePicture();
	void onVideoPathChanged(const QString& path);
	void onPicturePathChanged(const QString& path);

private:
	bool isFileExists(const QString& path);
	void addItem(const VideoItem& item);
	void modifyItem(const VideoItem& to);

private:
	Ui::VideoEditor ui;
	int modifying_;
};