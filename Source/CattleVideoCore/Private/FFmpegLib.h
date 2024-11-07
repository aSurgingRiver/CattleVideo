#pragma once
#include "Interfaces/IPluginManager.h"
#include <vector>

// WEB_CORE_API
class IFFmpegLIB {
public:
	static IFFmpegLIB* get();
	virtual void LoadCEF3Modules() = 0;
	virtual void UnloadCEF3Modules() = 0;
	virtual FString LibPath() = 0;
};


