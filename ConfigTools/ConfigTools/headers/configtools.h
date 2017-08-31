#pragma once

#include <QSet>
#include <QtWidgets/QDialog>
#include <QStandardItemModel>

#include "ui_configtools.h"

struct USBDisk;
struct VideoItem;
struct ClientItem;

class USBTools;
class ImportTask;
class VideoEditor;
class ClientEditor;
class CategoryEditor;
class QTreeWidgetItem;
class ImportProgress;

class ConfigTools : public QDialog {
	Q_OBJECT

public:
	ConfigTools(QWidget *parent = Q_NULLPTR);
	~ConfigTools();

protected:
	virtual void keyReleaseEvent(QKeyEvent* e);
	virtual bool nativeEvent(const QByteArray &eventType, void *message, long *result);

private slots:
	void onAddVideo();
	void onModifyVideo();
	void onRemoveVideo();
	void onEditCategory();

	void onAddClient();
	void onModifyClient();
	void onRemoveClient();

	void onVideoTableCustomContextMenuRequested();
	void onVideoItemDoubleClicked(int row, int column);

	void onClientTableCustomContextMenuRequested();
	void onClientItemDoubleClicked(int row, int column);

	void onDiskAdded(const USBDisk&);
	void onDiskRemoved(const USBDisk&);
	
	void reloadVideos();
	void reloadClients();

	void importFiles();
	void reloadFileTree();

	void onTreeItemChanged(QStandardItem* item);

	void onImportFinished();
	void onOverridePrompt(QString path);
	void onCurrentImportingChanged(QString name);

private:
	bool selectedTableRows(QTableWidget* table, QVector<int>* cont);

	void appendVideoRow(const VideoItem& item);
	void appendClientRow(const ClientItem& item);

	int fillFileTreeRecursively(const QString& path, QStandardItem *parent);

	void initializeUI();
	void connectSignals();

	bool collectSelected(QStringList& videos, QStringList& pictures);
	void collectSelectedRecursively(QStandardItem* root, QStringList& videos, QStringList& pictures);

	void onChildCheckStateChanged(QStandardItem * item);
	Qt::CheckState checkSibling(QStandardItem * item);
	void checkAllChild(QStandardItem * item, bool check);

private:
	VideoEditor* videoEditor_;
	ClientEditor* clientEditor_;
	CategoryEditor* categoryEditor_;

	QStandardItemModel* model_;

	USBDisk* currentDisk_;

	QSet<QString> selectedFiles_;

	ImportTask* task_;
	ImportProgress* progress_;

	USBTools* usbTools_;
	Ui::ConfigTools ui;
};
