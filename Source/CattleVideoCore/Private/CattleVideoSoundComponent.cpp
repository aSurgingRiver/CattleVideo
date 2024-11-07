// Fill out your copyright notice in the Description page of Project Settings.


#include "CattleVideoSoundComponent.h"
#include "CattleVideoSoundGenerator.h"
#include "CattleVideoControl.h"

/*
* Component that recieves audio from a remote webrtc connection and outputs it into UE using a "synth component".
*/
UCattleVideoSoundComponent::UCattleVideoSoundComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, SoundGenerator(MakeShared<FCattleVideoSoundGenerator, ESPMode::ThreadSafe>())
{
	PreferredBufferLength = 512u;
	NumChannels = 0;
	PrimaryComponentTick.bCanEverTick = true;
	SetComponentTickEnabled(true);
	bAutoActivate = true;
	SoundGenerator->OnSoundInit().BindUObject(this,&UCattleVideoSoundComponent::InitSound);
};

void UCattleVideoSoundComponent::SetContorl(TSharedPtr<UCattleVideoControl, ESPMode::ThreadSafe> Control) {
	//SoundGenerator->SetContorl(Control);
	Control->SetSound(SoundGenerator);
}

void UCattleVideoSoundComponent::Slient(bool S) {
	SoundGenerator->Slient(S);
}

void UCattleVideoSoundComponent::Volume(unsigned int value) {
	SoundGenerator->Volume(value);

}
void UCattleVideoSoundComponent::InitSound(int32 SampleRate, int32 Channels) {
	NumChannels = Channels;
	Stop();
	Initialize(SampleRate);
	Start();
}

ISoundGeneratorPtr UCattleVideoSoundComponent::CreateSoundGenerator(const FSoundGeneratorInitParams& InParams)
{
	SoundGenerator->SetParameters(InParams);
	return SoundGenerator;
}


ISoundGeneratorPtr UCattleVideoSoundComponent::CreateSoundGenerator(int32 InSampleRate, int32 InNumChannels)
{
	SoundGenerator->SetParameters(InSampleRate,InNumChannels);
	return nullptr;
	//return SoundGenerator;
}

int32 UCattleVideoSoundComponent::OnGenerateAudio(float* OutAudio, int32 NumSamples) {
#if (ENGINE_MAJOR_VERSION*10000+ENGINE_MINOR_VERSION*100)<50000
	return SoundGenerator->OnGenerateAudio(OutAudio, NumSamples);
#else
	return 0;
#endif
}

void UCattleVideoSoundComponent::BeginDestroy()
{
	if (SoundGenerator)SoundGenerator = nullptr;
	Super::BeginDestroy();
}

//bool UCattleVideoSoundComponent::Init(int32& SampleRate)
//{
//	NumChannels = 1;
//
////#if SYNTHCOMPONENT_EX_OSCILATOR_ENABLED
////	// Initialize the DSP objects
////	Osc.Init(SampleRate);
////	Osc.SetFrequency(440.0f);
////	Osc.Start();
////#endif // SYNTHCOMPONENT_EX_OSCILATOR_ENABLED
//
//	return true;
//}
//
//int32 UCattleVideoSoundComponent::OnGenerateAudio(float* OutAudio, int32 NumSamples)
//{
//#if SYNTHCOMPONENT_EX_OSCILATOR_ENABLED
//	// Perform DSP operations here
//	for (int32 Sample = 0; Sample < NumSamples; ++Sample)
//	{
//		OutAudio[Sample] = Osc.Generate();
//	}
//#endif // SYNTHCOMPONENT_EX_OSCILATOR_ENABLED
//
//	return NumSamples;
//}
//
//void UCattleVideoSoundComponent::SetFrequency(const float InFrequencyHz)
//{
//#if SYNTHCOMPONENT_EX_OSCILATOR_ENABLED
//	// Use this protected base class method to push a lambda function which will safely execute in the audio render thread.
//	SynthCommand([this, InFrequencyHz]()
//	{
//		Osc.SetFrequency(InFrequencyHz);
//		Osc.Update();
//	});
//#endif // SYNTHCOMPONENT_EX_OSCILATOR_ENABLED
//}