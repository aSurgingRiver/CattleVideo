// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CattleVideoActor.generated.h"

class UCattleVideoSoundComponent;
class UCattleVideoControl;
UCLASS()
class CATTLEVIDEOCORE_API ACattleVideoActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACattleVideoActor();
	void SetContorl(TSharedPtr<UCattleVideoControl, ESPMode::ThreadSafe> Control);
	void Slient(bool S);
	void Volume(unsigned int value = 100);
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void OnConstruction(const FTransform& Transform);
	//virtual TSharedPtr<UCattleVideoControl, ESPMode::ThreadSafe> Control;
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	UCattleVideoSoundComponent* SoundComponent;
};
