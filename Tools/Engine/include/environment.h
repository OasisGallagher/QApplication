#pragma once
#include "defs.h"

class ENGINE_EXPORT Environment {
public:
	static bool Initialize(fnLogCallback cb);

private:
	static void __stdcall DebugOutputCallback(
		GLenum source,
		GLenum type, 
		GLuint id, 
		GLenum severity, 
		GLsizei length, 
		const GLchar* message, 
		const GLvoid* userParam
	);
};
