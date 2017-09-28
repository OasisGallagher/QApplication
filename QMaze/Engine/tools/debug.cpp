#include <ctime>
#include <string>
#include <iostream>
#include <sstream>
#include <fstream>
#include <gl/glew.h>

#include "debug.h"
#include "tools/mathf.h"
#include "tools/string.h"

int Debug::length_ = 0;
std::stack<std::string> Debug::samples_;
LogCallback Debug::fnptr_ = nullptr;

std::ofstream debug("main/debug/debug.txt");

void Debug::SetLogCallback(LogCallback cb) {
	fnptr_ = cb;
}

void Debug::Log(const std::string& text) {
	if (fnptr_ != nullptr) { fnptr_(LogLevelDebug, text.c_str()); }
}

void Debug::LogWarning(const std::string& text) {
	if (fnptr_ != nullptr) { fnptr_(LogLevelWarning, text.c_str()); }
}

void Debug::LogError(const std::string& text) {
	if (fnptr_ != nullptr) { fnptr_(LogLevelError, text.c_str()); }
}

std::string Debug::Now() {
	time_t now = time(nullptr);
	tm* ptr = localtime(&now);
	return String::Format("%02d:%02d:%02d", ptr->tm_hour, ptr->tm_min, ptr->tm_sec);
}

void Debug::Break(const std::string& expression, const char* file, int line) {
	std::ostringstream oss;
	oss << expression << "\n";
	oss << "at " << file << ":" << line;
	if (fnptr_ != nullptr) { fnptr_(LogLevelFatal, oss.str().c_str()); }
}

void Debug::Break(const std::string& expression, const std::string& message, const char* file, int line) {
	std::ostringstream oss;
	oss << expression + ":\n" + message << "\n";
	oss << "at " << file << ":" << line;
	if (fnptr_ != nullptr) { fnptr_(LogLevelFatal, oss.str().c_str()); }
}

void Debug::EnableMemoryLeakCheck() {
	//OS::EnableMemoryLeakCheck();
}

void Debug::StartSample(const std::string& text) {
	Debug::Log("\"" + text + "\" at " + Now() + ".");

	clock_t now = clock();
	samples_.push(text + "@" + std::to_string(now));
}

void Debug::EndSample() {
	std::string samp = samples_.top();
	int pos = samp.rfind('@');
	samples_.pop();
	clock_t elapsed = clock() - atol(samp.c_str() + pos + 1);
	samp[pos] = 0;
	Debug::Log(String::Format("\"%s\" costs %.2f seconds.", samp.c_str(), ((float)elapsed / CLOCKS_PER_SEC)));
}

void Debug::AssertGLImpl(const char* file, int line) {
	GLenum err = glGetError(); 
	if (err != GL_NO_ERROR) {
		Break(String::Format("glError: 0x%04x.", err), file, line);
	}
}
