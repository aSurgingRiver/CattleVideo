// Fill out your copyright notice in the Description page of Project Settings.


#include "CattleVideoActor.h"
#include "CattleVideoSoundComponent.h"

namespace cattlevideo {
	void MakeComponent(UCattleVideoSoundComponent*& SoundComponent,UObject* outer) {
		if (SoundComponent)return;
		SoundComponent = NewObject<UCattleVideoSoundComponent>(outer, NAME_None, RF_Transactional | RF_Transient | RF_TextExportTransient);
	}
}

// Sets default values
ACattleVideoActor::ACattleVideoActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	SoundComponent = nullptr;
}


void ACattleVideoActor::OnConstruction(const FTransform& Transform) {
	cattlevideo::MakeComponent(SoundComponent,this);
}

// Called when the game starts or when spawned
void ACattleVideoActor::BeginPlay()
{
	Super::BeginPlay();
	cattlevideo::MakeComponent(SoundComponent, this);
}

void ACattleVideoActor::SetContorl(TSharedPtr<UCattleVideoControl, ESPMode::ThreadSafe> Control) {
	cattlevideo::MakeComponent(SoundComponent, this); 
	SoundComponent->SetContorl(Control);
}

void ACattleVideoActor::Slient(bool S) {
	cattlevideo::MakeComponent(SoundComponent, this); 
	SoundComponent->Slient(S);
}

void ACattleVideoActor::Volume(unsigned int value) {
	cattlevideo::MakeComponent(SoundComponent, this); 
	SoundComponent->Volume(value);
}
// Called every frame
void ACattleVideoActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

