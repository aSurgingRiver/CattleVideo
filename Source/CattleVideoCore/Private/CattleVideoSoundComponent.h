// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SynthComponent.h"
#include "DSP/Osc.h"
#include "CattleVideoSoundGenerator.h"
#include "CattleVideoSoundComponent.generated.h"
//class FCattleVideoSoundGenerator;

// ========================================================================
// UCattleVideoSoundComponent
// Synth component class which implements USynthComponent
// This is a simple hello-world type example which generates a sine-wave
// tone using a DSP oscillator class and implements a single function to set
// the frequency. To enable example:
// 1. Ensure "SignalProcessing" is added to project's .Build.cs in PrivateDependencyModuleNames
// 2. Enable macro below that includes code utilizing SignalProcessing Oscilator
// ========================================================================

#define SYNTHCOMPONENT_EX_OSCILATOR_ENABLED 0

UCLASS(ClassGroup = Synth, meta = (BlueprintSpawnableComponent))
class CATTLEVIDEOCORE_API UCattleVideoSoundComponent : public USynthComponent
{
	GENERATED_UCLASS_BODY()
	
	// Called when synth is created
	//virtual bool Init(int32& SampleRate) override;

	//// Called to generate more audio
	//virtual int32 OnGenerateAudio(float* OutAudio, int32 NumSamples) override;

	//// Sets the oscillator's frequency
	//UFUNCTION(BlueprintCallable, Category = "Synth|Components|Audio")
	//void SetFrequency(const float FrequencyHz = 440.0f);
public:
	void SetContorl(TSharedPtr<UCattleVideoControl, ESPMode::ThreadSafe> Control);
	void Slient(bool S);
	void Volume(unsigned int value = 100);
protected:
	void InitSound(int32 SampleRate,int32 Channels);
	ISoundGeneratorPtr CreateSoundGenerator(const FSoundGeneratorInitParams& InParams);
	ISoundGeneratorPtr CreateSoundGenerator(int32 InSampleRate, int32 InNumChannels);
	int32 OnGenerateAudio(float* OutAudio, int32 NumSamples);
	void BeginDestroy();
protected:
//#if SYNTHCOMPONENT_EX_OSCILATOR_ENABLED
//	// A simple oscillator class. Can also generate Saw/Square/Tri/Noise.
//	Audio::FOsc Osc;
//#endif // SYNTHCOMPONENT_EX_OSCILATOR_ENABLED
	TSharedPtr<FCattleVideoSoundGenerator, ESPMode::ThreadSafe> SoundGenerator;
};