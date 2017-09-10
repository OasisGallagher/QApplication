#pragma once
#include <stack>
#include <string>

typedef void(*LogCallback)(int level, const char* message);

class Debug {
	enum {
		LogLevelDebug,
		LogLevelWarning,
		LogLevelError,
		LogLevelFatal,
	};

public:
	static void SetLogCallback(LogCallback cb);

	static void Log(const std::string& text);
	static void LogWarning(const std::string& text);
	static void LogError(const std::string& text);

	static std::string Now();
	static void Break(const std::string& expression, const char* file, int line);
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
	static LogCallback fnptr_;

};

#define Verify(expression) \
	(void)((!!(expression)) || (Debug::Break(#expression,  __FILE__, __LINE__), 0))

#define VerifyX(expression, message)	\
	(void)((!!(expression)) || (Debug::Break(#expression, message,  __FILE__, __LINE__), 0))

#if _DEBUG
#define Assert(expression)	Verify(expression)
#define AssertX(expression, message) VerifyX(expression, message)
#else
	(void)0
#endif
