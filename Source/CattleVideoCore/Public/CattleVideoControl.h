// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Slate/SlateTextures.h"
#include "Textures/SlateShaderResource.h"
#include <memory>
struct FCattleVideoOption;
class FSlateShaderResource;
class FCattleVideoSoundGenerator;
/**
 * 
 */

namespace cattlevideo {
	struct VideoSize {
		std::atomic<int>  width;
		std::atomic<int>  height;
	};
}

class CATTLEVIDEOCORE_API UCattleVideoControl
{
public:
	DECLARE_MULTICAST_DELEGATE_ThreeParams(FOnLoadStatus, FString, int, FString);
	DECLARE_MULTICAST_DELEGATE_TwoParams(FOnTextureSync, UTexture2D*, UTexture2D*);
	virtual FOnLoadStatus& OnLoadStatus()=0;
	virtual FOnTextureSync& OnTextureSync() = 0;
	virtual UTexture2D* GetTexture2D()=0;
	virtual void Close();
	virtual bool Open(FString URL, FCattleVideoOption& option);
	virtual void SetSound(TSharedPtr<FCattleVideoSoundGenerator, ESPMode::ThreadSafe> SoundGenerator);
	virtual void Pause();
	virtual void Resume();
	virtual void Speed(uint32);
	virtual void AddSize(std::shared_ptr<cattlevideo::VideoSize> size);
	virtual void DelSize(std::shared_ptr<cattlevideo::VideoSize> size);
	virtual int  Duration();
	virtual bool Seek(int ms);
	virtual void ProgressSynchro(bool synchro);
	virtual ~UCattleVideoControl();
	UCattleVideoControl();
};

namespace cattlevideo {
	CATTLEVIDEOCORE_API TSharedPtr<UCattleVideoControl, ESPMode::ThreadSafe> GetControl();
	CATTLEVIDEOCORE_API void OnBeginPIE(bool);
	CATTLEVIDEOCORE_API void OnEndPIE(bool);
	CATTLEVIDEOCORE_API void WritePic(int Height, int Width, void* data, FString prefix, int index);

}