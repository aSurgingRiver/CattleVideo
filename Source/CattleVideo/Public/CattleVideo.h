// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/Widget.h"
#include <memory>
#include "CattleVideoOption.h"
#include "CattleVideo.generated.h"

class FCattleVideoViewport;
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

/**
 * 
 */
UCLASS()
class CATTLEVIDEO_API UCattleVideo : public UWidget
{
	GENERATED_UCLASS_BODY()
public:
	/**
	 * Open the specified URL or file
	 */
	UFUNCTION(BlueprintCallable, Category = "Ant Cattle")
	bool Open(FString URL, FCattleVideoOption Option);
	/**
	 * Close vedio
	 */
	UFUNCTION(BlueprintCallable, Category = "Ant Cattle")
	void Close();
	/**
	 * Default Option
	 */
	UFUNCTION(BlueprintCallable, Category = "Ant Cattle")
	FCattleVideoOption DefaultOption();

	void CopyFrom(UCattleVideo* From);

	virtual void BeginDestroy() override;

protected:
	virtual TSharedRef<SWidget> RebuildWidget() override;
private:
	TSharedPtr<FCattleVideoViewport> Viewport;
	TSharedPtr<UCattleVideoControl, ESPMode::ThreadSafe> Control;
};
