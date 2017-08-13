#include <io.h>
#include <fcntl.h>

#include "Algorithm.h"

#define SIZE	8192

#ifndef S_IRWXU
#define S_IRWXU	(_S_IREAD | _S_IWRITE)
#endif

#ifndef S_IRGRP
#define S_IRGRP 0
#endif

#ifndef S_IROTH
#define S_IROTH 0
#endif

#define MAGIC  "Secret"

qint64 ReverseAlgorithm::calculateOutFileSize(qint64 in_size) {
	return strlen(MAGIC) + in_size;
}

int ReverseAlgorithm::encrypt(const char* in, const char* out) {
	int fd0 = -1;
	int fd1 = -1;
	int i = 0;
	int j = 0;
	char temp[SIZE] = { 0 };
	int len;
	char convert;

	fd0 = open(in, O_RDONLY | O_BINARY);
	if (fd0 <= 0) {
		return errno;
	}

	fd1 = open(out, O_WRONLY | O_CREAT | O_TRUNC | O_BINARY, S_IRWXU | S_IRGRP | S_IROTH);

	if (fd1 <= 0) {
		return errno;
	}

	chmod(out, S_IRWXU | S_IRGRP | S_IROTH);

	write(fd1, "Secret", 6);

	while (!interrupted() && (len = read(fd0, temp, SIZE)) > 0) {
		for (i = 0; i < len; i++) {
			convert = 0;

			for (j = 0; j < 8; j++) {
				convert = (convert << 1) | (temp[i] & 1);
				temp[i] = temp[i] >> 1;
			}

			temp[i] = convert;
		}

		write(fd1, temp, len);
		memset(temp, 0, SIZE);
	}

	close(fd0);
	close(fd1);

	return 0;
}

