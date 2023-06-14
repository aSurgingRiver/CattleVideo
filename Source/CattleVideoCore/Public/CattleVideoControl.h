// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Slate/SlateTextures.h"
#include "Textures/SlateShaderResource.h"
struct FCattleVideoOption;
class FSlateShaderResource;

/**
 * 
 */
class CATTLEVIDEOCORE_API UCattleVideoControl
{
public:
	virtual FSlateShaderResource* GetTargetTexture();
	virtual void Close();
	virtual bool Open(FString URL, FCattleVideoOption& option);
	virtual ~UCattleVideoControl();
	UCattleVideoControl();
};

namespace cattlevideo {
	CATTLEVIDEOCORE_API TSharedPtr<UCattleVideoControl, ESPMode::ThreadSafe> GetControl();
	CATTLEVIDEOCORE_API void OnBeginPIE(bool);
	CATTLEVIDEOCORE_API void OnEndPIE(bool);
	CATTLEVIDEOCORE_API void WritePic(int Height, int Width, void* data, FString prefix, int index);

}