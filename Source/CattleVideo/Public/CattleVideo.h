// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/Widget.h"
#include <memory>
#include "Delegates/DelegateCombinations.h"
#include "Containers/Map.h"
#include "CattleVideoOption.h"
#include "CattleVideo.generated.h"

class ACattleVideoActor;
class SCattleVideoWidget;
class UCattleVideoControl;
struct FCattleVideoOption;
//UCLASS()
//class CattleVideo_API UCattleVideoCopy : public UObject
//{
//	GENERATED_UCLASS_BODY()
//public:
//	std::shared_ptr<UCattleVideoControl>& Control();
//private:
//	std::shared_ptr<UCattleVideoControl> _Control;
//};

UENUM(BlueprintType, Blueprintable)
enum class CATTLE_VIDEO_SPEED : uint8{
	CATTLE_VIDEO_SPEED_050  UMETA(DisplayName = "0.5x"),     // 0.5
	CATTLE_VIDEO_SPEED_075  UMETA(DisplayName = "0.75x"),     // 0.75
	CATTLE_VIDEO_SPEED_100  UMETA(DisplayName = "1.0x"),    // 1.0
	CATTLE_VIDEO_SPEED_125  UMETA(DisplayName = "1.25x"),    // 1.25
	CATTLE_VIDEO_SPEED_150  UMETA(DisplayName = "1.5x"),    // 1.5
	CATTLE_VIDEO_SPEED_175  UMETA(DisplayName = "1.75x"),    // 1.75
	CATTLE_VIDEO_SPEED_200  UMETA(DisplayName = "2.0x"),    // 2
	CATTLE_VIDEO_SPEED_225  UMETA(DisplayName = "2.25x"),    // 2.25
	CATTLE_VIDEO_SPEED_250  UMETA(DisplayName = "2.5x"),    // 2.5
	CATTLE_VIDEO_SPEED_300  UMETA(DisplayName = "3.0x"),    // 3
};

/**
 * 
 */
UCLASS(BlueprintType, Blueprintable)
class CATTLEVIDEO_API UCattleVideo : public UWidget
{
	GENERATED_UCLASS_BODY()
public:
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnLoadStatus, FString, URL, int, status,FString,Desc);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnTextureSync, UTexture2D*, NEW, UTexture2D*, OLD);
public:
	UPROPERTY(BlueprintAssignable, Category = "Cattle Video")
	FOnLoadStatus OnLoadStatus;

	UPROPERTY(BlueprintAssignable, Category = "Cattle Video")
	FOnTextureSync OnTextureSync;
	/**
	 * Open the specified URL or file
	 */
	UFUNCTION(BlueprintCallable, Category = "Cattle Video")
	bool Open(FString URL, FCattleVideoOption Option);
	/**
	 * Close vedio
	 */
	UFUNCTION(BlueprintCallable, Category = "Cattle Video")
	void Close();
	/**
	 * Default Option
	 */
	UFUNCTION(BlueprintCallable, Category = "Cattle Video")
	FCattleVideoOption DefaultOption();

	UFUNCTION(BlueprintCallable, Category = "Cattle Video")
	void Slient(bool YesNo=false);

	UFUNCTION(BlueprintCallable, Category = "Cattle Video")
	void Pause();

	UFUNCTION(BlueprintCallable, Category = "Cattle Video")
	void Resume();

	/**
	 * Sound regulation
	 * value : Zoom factor,if zero will Turn off sound.
	 *         100 No sound amplification or reduction .
	 *         Max 500
	 */
	UFUNCTION(BlueprintCallable, Category = "Cattle Video")
	void Volume(int value=100);

	/**
	 * Sound speed
	 * value : Zoom factor,if zero will Turn off sound.
	 *         100 No sound amplification or reduction .
	 *         Max 500
	 */
	UFUNCTION(BlueprintCallable, Category = "Cattle Video")
	void Speed(CATTLE_VIDEO_SPEED value= CATTLE_VIDEO_SPEED::CATTLE_VIDEO_SPEED_100);


	/**
	 * Get video total duration ,unit ms
	 */
	UFUNCTION(BlueprintCallable, Category = "Cattle Video")
	int Duration() const;

	/**
	 * Seek video to postion ,unit ms
	 */
	UFUNCTION(BlueprintCallable, Category = "Cattle Video")
	bool Seek(int ms);


	/**
	 * get current texture
	 */
	UFUNCTION(BlueprintCallable, Category = "Cattle Video")
	UTexture2D* Texture2D();

	void CopyFrom(UCattleVideo* From);

	virtual void BeginDestroy() override;

protected:
	virtual TSharedRef<SWidget> RebuildWidget() override;

	void OnLoadStatusH(FString URL,int status,FString Desc);
	void OnTextureSyncH(UTexture2D* NEW,UTexture2D* OLD);
	void BindHandle();
	void UnBindHandle();
	ACattleVideoActor* GetSoundActor();
private:
	TSharedPtr<SCattleVideoWidget> Widget;
	TSharedPtr<UCattleVideoControl, ESPMode::ThreadSafe> Control;
	ACattleVideoActor* SoundActor;
	FDelegateHandle load_status_handle;
	FDelegateHandle texture_sync_handle;
};
