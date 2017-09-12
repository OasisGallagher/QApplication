#pragma once
#include "world.h"
#include "defines.h"

enum {
	LogLevelDebug,
	LogLevelWarning,
	LogLevelError,
	LogLevelFatal,
};

typedef void(*LogCallback)(int level, const char* message);

class ENGINE_EXPORT Engine {
public:
	Engine() : world_(nullptr) {
	}

public:
	static Engine* Ptr() {
		return &engine_;
	}

public:
	bool Initialize();
	void SetDebugCallback(LogCallback callback);

	void Update();
	World WorldPtr() { return world_; }

private:
	World world_;

	static Engine engine_;
};
