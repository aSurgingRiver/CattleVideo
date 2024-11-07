
#pragma once

#include "CoreMinimal.h"

class CCattleConfig {
public:
	static CCattleConfig* get();
	bool FastPlay(bool& fast);
	bool AutoSize(bool& fast);
private:
	void Init();
private:
	TOptional<bool> fast_play;
	TOptional<bool> auto_size;
};

