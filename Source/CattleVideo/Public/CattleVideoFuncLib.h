// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "CattleVideoFuncLib.generated.h"

class UCattleVideo;
/**
 * 
 */
UCLASS()
class CATTLEVIDEO_API UCattleVideoFuncLib : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, Category = "Cattle Video")
	static UCattleVideo* Copy(UCattleVideo* from, UCattleVideo* to);
};
