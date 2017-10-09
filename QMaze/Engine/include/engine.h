#pragma once
#include "world.h"
#include "defines.h"

enum {
	LogLevelDebug,
	LogLevelWarning,
	LogLevelError,
	LogLevelFatal,
};

class ILogCallback {
public:
	virtual void OnEngineLogMessage(int level, const char* message) = 0;
};

class Time;
class Screen;
class ILogCallback;

class ENGINE_EXPORT Engine {
public:
	Engine() : world_(nullptr) {
	}

public:
	static Engine* get() { return &engine_; }

public:
	bool initialize();
	void release();

	/**
	 * make debug context !!!
	 */
	void setDebugCallback(ILogCallback* callback);

	void update();
	void onResize(int w, int h);

	World world() { return world_; }

	float realTimeSinceStartup();
	float deltaTime();
	int frameCount();

	int contextWidth();
	int contextHeight();

private:
	World world_;
	Time* time_;
	Screen* screen_;

	static Engine engine_;
};
