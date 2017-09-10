#pragma once
#include <cstdarg>
#include <string>
#include "tools/debug.h"

class String {
public:
	static std::string Trim(const std::string& text);
	static std::string Format(const char* format, ...);

private:
	String();
};

inline std::string String::Format(const char* format, ...) {
	const int formatBufferLength = 512;
	static char formatBuffer[formatBufferLength];

	va_list ap;
	va_start(ap, format);
	int n = vsnprintf(formatBuffer, formatBufferLength, format, ap);
	va_end(ap);

	AssertX(n >= 0 && n < formatBufferLength, "format error");

	return formatBuffer;
}

inline std::string String::Trim(const std::string& text) {
	const char* whitespaces = " \t";
	size_t left = text.find_first_not_of(whitespaces);
	size_t right = text.find_last_not_of(whitespaces);
	if (left == std::string::npos || right == std::string::npos) {
		return text;
	}

	return std::string(text.begin() + left, text.begin() + right + 1);
}
