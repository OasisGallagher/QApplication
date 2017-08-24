#include <QMenu>
#include <QDebug>
#include <QMovie>
#include <QFileInfo>
#include <QMessageBox>
#include <QFileDialog>
#include <QProgressBar>
#include <QTranslator>
#include <QStyledItemDelegate>

#include "Utility.h"
#include "Application.h"

enum {
	ColumnName,
	ColumnSize,
	ColumnProgress,
	ColumnCount
};

Application::Application(QWidget *parent)
	: QDialog(parent) {
	ui.setupUi(this);

	initWindowWidgets();

	connect(ui.openFile, SIGNAL(clicked()), this, SLOT(onClickOpenFile()));
	connect(ui.openFolder, SIGNAL(clicked()), this, SLOT(onClickOpenFolder()));

	timer_id_ = startTimer(500);
}

Application::~Application() {
	killTimer(timer_id_);
	foreach(Task task, tasks_) {
		task.encrypter->interrupt();
		delete task.encrypter;
	}
}

bool Application::addTask(const QString& path, Encrypter* encrypter) {
	if (findTask(path) >= 0) {
		return false;
	}

	QTableWidget* table = ui.tableWidget;
	int index = tasks_.size();

	table->insertRow(index);
	table->setRowHeight(index, 22);

	QFileInfo info(path);
	QString name = info.fileName();
	QString size = Utility::encodeSize(info.size());
	
	name = QString::fromUtf8(name.toLatin1());
	table->setItem(index, ColumnName, new QTableWidgetItem(name));

	QTableWidgetItem* size_item = new QTableWidgetItem(size);
	size_item->setTextAlignment(Qt::AlignRight);
	table->setItem(index, ColumnSize, size_item);

	QProgressBar* progress = new QProgressBar(table);
	progress->setTextVisible(true);
	progress->setStyleSheet("text-align:center;font-family:Microsoft YaHei");
	table->setCellWidget(index, ColumnProgress, progress);
	
	Task task = { path, encrypter };
	tasks_.append(task);

	return true;
}

bool Application::removeTask(const QString& path) {
	int index = findTask(path);
	if (index < 0) {
		return false;
	}

	tasks_.removeAt(index);
	ui.tableWidget->removeRow(index);

	return true;
}

int Application::findTask(const QString& path) {
	int index = -1;
	for (int i = 0; i < tasks_.size(); ++i) {
		if (tasks_[i].path == path) {
			index = i;
			break;
		}
	}

	return index;
}
void Application::setProgress(const QString& path, int value) {
	int index = findTask(path);

	if (index >= 0) {
		QProgressBar* pb = (QProgressBar*)ui.tableWidget->cellWidget(index, ColumnProgress);
		pb->setValue(value);
	}
}

void Application::initWindowWidgets() {
	setWindowFlags(windowFlags() | Qt::WindowMinMaxButtonsHint);

	QMovie* movie = new QMovie(":/images/loading");
	ui.loading->setMovie(movie);
	movie->start();

	QTableWidget* table = ui.tableWidget;
	table->setColumnCount(ColumnCount);
	
	QHeaderView* hHeader = table->horizontalHeader();

	hHeader->setSectionsClickable(false);
	hHeader->setStretchLastSection(true);
	hHeader->setFixedHeight(25);

	table->setSelectionBehavior(QAbstractItemView::SelectItems);

	QStringList headers;
	headers << tr("TableTitleFile") << tr("TableTitleSize") << tr("TableTitleProgress");
	table->setHorizontalHeaderLabels(headers);

	table->setFrameShape(QFrame::NoFrame);

	QHeaderView* vHeader = table->verticalHeader();
	vHeader->hide();

	hHeader->resizeSection(0, 150);

	//table->setItemDelegate(new NoFocusDelegate());
	table->setMouseTracking(true);
	table->setStyleSheet("selection-background-color:lightblue;border-width: 1px; border-style: solid;border-color: lightgray;");

	hHeader->setStyleSheet("font-weight:bold;");
}

void Application::onClickOpenFile() {
	QStringList files = QFileDialog::getOpenFileNames(this, tr("DialogTitleOpenFiles"));
	if (!files.isEmpty()) {
		encryptFiles(files);
	}
}

void Application::onClickOpenFolder() {
	QString folder = QFileDialog::getExistingDirectory(this, tr("DialogTitleOpenFolder"));
	if (!folder.isEmpty()) {
		encryptFiles(enumFiles(folder));
	}
}

void Application::timerEvent(QTimerEvent *event) {
	if (event->timerId() != timer_id_) { return; }

	foreach(Task task, tasks_) {
		setProgress(task.path, task.encrypter->progress());
	}

	if (tasks_.isEmpty()) {
		ui.openers->setEnabled(true);
	}
}

void Application::contextMenuEvent(QContextMenuEvent* event) {
	QMenu* menu = new QMenu(this);
	QMenu* sub_menu = menu->addMenu(tr("Language"));
	QAction* action = sub_menu->addAction(language_ == Chinese ? tr("English") : tr("Chinese"));
	
	connect(action, SIGNAL(triggered()), this, SLOT(onToggleLanguage()));

	menu->exec(cursor().pos());
}

void Application::onToggleLanguage() {
	QString path;
	if (language_ == Chinese) {
		language_ = English;
		path = ":qm/en";
	}
	else {
		language_ = Chinese;
		path = ":qm/zh";
	}

	QTranslator translator;

	translator.load(path);
	qApp->installTranslator(&translator);

	ui.retranslateUi(this);
}

QStringList Application::enumFiles(const QString& path) {
	QStringList answer;

	QDir dir(path);
	QFileInfoList files = dir.entryInfoList(QDir::Files | QDir::NoSymLinks);
	QFileInfoList folders = dir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot);

	for (int i = 0; i < files.size(); ++i) {
		answer.append(files[i].absoluteFilePath());
	}

	for (int i = 0; i != folders.size(); i++) {
		QStringList children = enumFiles(folders[i].absoluteFilePath());
		answer.append(children);
	}

	return answer;
}

void Application::onThreadFinished(QString path) {
	mutex_.lock();

	int index = findTask(path);
	if (index >= 0) {
		tasks_[index].encrypter->deleteLater();
		removeTask(path);
	}

	mutex_.unlock();
}

void Application::encryptFiles(const QStringList& paths) {
	ui.openers->setEnabled(false);

	for (int i = 0; i < paths.size(); ++i) {
		if (findTask(paths[i]) >= 0) {
			continue;
		}

		Encrypter* encrypter = new Encrypter(this);

		addTask(paths[i], encrypter);
		connect(encrypter, SIGNAL(finished(QString)), this, SLOT(onThreadFinished(QString)));
		encrypter->encrypt(paths[i]);
	}
}
