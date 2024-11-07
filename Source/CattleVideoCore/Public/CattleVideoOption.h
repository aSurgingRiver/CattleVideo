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
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "Package Cache Number", UIMin = 30), Category = "Cattle Video")
	int PkgCacheNumber;

	/** Package Cache Number */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "Wait Time", UIMin = 1), Category = "Cattle Video")
	int WaitTime;

	/** Package Cache Number */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "Hard ware"), Category = "Cattle Video")
	bool Hardware;

	/** Repeat Play */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "Repeat Play"), Category = "Cattle Video")
	bool Repeat;

	/** Repeat Play */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "Silent"), Category = "Cattle Video")
	bool Silent;

	/** Repeat Play */
	//UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "Auto Size"), Category = "Cattle Video")
	//bool AutoSize;

	/**
	* param for avformat_open_input
	* timeout 1000 ms
	* scan_all_pmts 1
	**/
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (DisplayName = "FFmpeg Dictionary"), Category = "Cattle Video")
	TMap<FString, FString> FFmpegDictionary;

	FCattleVideoOption();
};
