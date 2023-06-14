// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "CattleVideo.h"
#include "CattleVideoFuncLib.generated.h"

/**
 * 
 */
UCLASS()
class CATTLEVIDEO_API UCattleVideoFuncLib : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, Category = "Ant Cattle")
	static UCattleVideo* Copy(UCattleVideo* from, UCattleVideo* to);
};
