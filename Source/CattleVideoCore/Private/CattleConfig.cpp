

#include "CattleConfig.h"
#include "Misc/ConfigCacheIni.h"

CCattleConfig* CCattleConfig::get() {
	static CCattleConfig* instanse = nullptr;
	if (!instanse) { instanse = new CCattleConfig(); instanse->Init(); }
	return instanse;
}

void CCattleConfig::Init() {
	bool bValue=false;
	if (GConfig->GetBool(TEXT("CattleVideo"), TEXT("fast_play"), bValue, GGameIni)) {
		fast_play = bValue;
	}
	bValue = false;
	if (GConfig->GetBool(TEXT("CattleVideo"), TEXT("auto_size"), bValue, GGameIni)) {
		auto_size = bValue;
	}
}

bool CCattleConfig::FastPlay(bool& fast) {
	if (!fast_play.IsSet())return fast;
	fast = fast_play.GetValue();
	return fast;
}

bool CCattleConfig::AutoSize(bool& autosize) {
	if (!auto_size.IsSet())return autosize;
	autosize = auto_size.GetValue();
	return autosize;
}