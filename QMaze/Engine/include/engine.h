#pragma once
#include "world.h"
#include "defines.h"

#include "timef.h"
#include "screen.h"
#include "graphics.h"

enum {
	LogLevelDebug,
	LogLevelWarning,
	LogLevelError,
	LogLevelFatal,
};

class EngineLogCallback {
public:
	virtual void OnEngineLogMessage(int level, const char* message) = 0;
};

class EngineLogCallback;

class ENGINE_EXPORT Engine {
public:
	Engine() : world_(nullptr) {
	}

public:
	static Engine* get();

public:
	bool initialize();
	void release();

	/**
	 * TODO: make debug context !!!
	 */
	void setLogCallback(EngineLogCallback* callback);

	void update();
	void resize(int w, int h);

	World world() { return world_; }

	Time* time();
	Screen* screen();
	Graphics* graphics();

private:
	World world_;
};
