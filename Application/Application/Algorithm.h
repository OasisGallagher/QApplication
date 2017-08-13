#pragma once
#include <QObject>

class Algorithm {
public:
	Algorithm() : interrupted_(false) {}

public:
	virtual int encrypt(const char* in, const char* out) = 0;
	virtual qint64 calculateOutFileSize(qint64 in) = 0;

public:
	void interrupt() { interrupted_ = true; }
	bool interrupted() const { return interrupted_; }

private:
	bool interrupted_;
};

class ReverseAlgorithm : public Algorithm {
public:
	virtual int encrypt(const char* in, const char* out);
	virtual qint64 calculateOutFileSize(qint64 in);
};
