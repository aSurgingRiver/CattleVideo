// Fill out your copyright notice in the Description page of Project Settings.


#include "CattleVideoFuncLib.h"
#include "CattleVideo.h"


UCattleVideo* UCattleVideoFuncLib::Copy(UCattleVideo* from, UCattleVideo* to) {
	if(to)to->CopyFrom(from);
	return to;
}