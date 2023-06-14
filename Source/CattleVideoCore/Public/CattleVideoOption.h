// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "CattleVideoOption.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType, Blueprintable)
struct CATTLEVIDEOCORE_API FCattleVideoOption
{
	GENERATED_USTRUCT_BODY()
public:
	/** Package Cache Number */
	UPROPERTY(BlueprintReadWrite, meta = (DisplayName = "Package Cache Number", UIMin = 30), Category = "Ant Vedio")
	int PkgCacheNumber;

	/** Package Cache Number */
	UPROPERTY(BlueprintReadWrite, meta = (DisplayName = "Wait Time", UIMin = 1), Category = "Ant Vedio")
	int WaitTime;

	/** Package Cache Number */
	UPROPERTY(BlueprintReadWrite, meta = (DisplayName = "Hard ware"), Category = "Ant Vedio")
	bool Hardware;

	/** Repeat Play */
	UPROPERTY(BlueprintReadWrite, meta = (DisplayName = "Repeat Play"), Category = "Ant Vedio")
	bool Repeat;

	/**
	* param for avformat_open_input
	* timeout 1000 ms
	* scan_all_pmts 1
	**/
	UPROPERTY(BlueprintReadWrite, meta = (DisplayName = "FFmpeg Dictionary"), Category = "Ant Vedio")
	TMap<FString, FString> FFmpegDictionary;

	FCattleVideoOption();
};
