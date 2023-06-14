// Fill out your copyright notice in the Description page of Project Settings.


#include "CattleVideoFuncLib.h"


UCattleVideo* UCattleVideoFuncLib::Copy(UCattleVideo* from, UCattleVideo* to) {
	to->CopyFrom(from);
	return to;
}