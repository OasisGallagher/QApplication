#include <QDebug>
#include <QThread>
#include <QFileInfo>

#include "Encrypter.h"
#include "Algorithm.h"

Encrypter::Encrypter(QObject* app)
	: app_(app)
	, file_size_(0)
	, progress_(0) {
	algorithm_ = new ReverseAlgorithm;
}

Encrypter::~Encrypter() {
	delete algorithm_;
}

int Encrypter::progress() const {
	QFileInfo out(out_path_);
	return (int)((double)out.size() / file_size_ * 100);
}

void Encrypter::encrypt(const QString& path) {
	path_ = path;
	start();
}

void Encrypter::run() {
	QFileInfo info(path_);
	file_size_ = info.size();

	out_path_ = info.path() + "/drm_" + info.fileName();
	QByteArray in = path_.toLocal8Bit();
	QByteArray out = out_path_.toLocal8Bit();

	out_size_ = algorithm_->calculateOutFileSize(file_size_);

	algorithm_->encrypt(in, out);
	emit finished(path_);
}

void Encrypter::interrupt() {
	algorithm_->interrupt();
	quit();
	wait();
}
