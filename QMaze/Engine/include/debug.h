#pragma once
#include <stack>
#include <string>

#include "defs.h"

class Debug {
public:
	static void SetLogCallback(fnLogCallback cb);

	static void Log(const std::string& text);
	static void LogWarning(const std::string& text);
	static void LogError(const std::string& text);

	static std::string Now();
	static void Break(const std::string& expression, const std::string& message, const char* file, int line);

	static void EnableMemoryLeakCheck();

	static void StartSample(const std::string& text);
	static void EndSample();

	static void StartProgress();
	static void LogProgress(const char* text, int current, int total);
	static void EndProgress();

private:
	Debug();

private:
	static int length_;
	static std::stack<std::string> samples_;
	static fnLogCallback fnptr_;

};

#define Verify(expression, message)	\
	(void)((!!(expression)) || (Debug::Break(#expression, message,  __FILE__, __LINE__), 0))

#if _DEBUG
#define Assert(expression, message)	\
	Verify(expression, message)
#else
	(void)0
#endif
