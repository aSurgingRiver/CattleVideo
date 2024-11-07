// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Sound/SoundGenerator.h"
#include "Runtime/Launch/Resources/Version.h"
class UCattleVideoControl;

// Parameters used for constructing a new ISoundGenerator.
#if (ENGINE_MAJOR_VERSION*10000+ENGINE_MINOR_VERSION*100)<50000
struct FSoundGeneratorInitParams
{
	Audio::FDeviceId AudioDeviceID;
	float SampleRate = 0.0f;
	int32 AudioMixerNumOutputFrames = 0;
	int32 NumChannels = 0;
	int32 NumFramesPerCallback = 0;
	uint64 InstanceID = 0;
	bool bIsPreviewSound = false;
	FString GraphName;
};
#endif
/*
* An `ISoundGenerator` implementation to pump some audio from WebRTC into this synth component
*/
class CATTLEVIDEOCORE_API FCattleVideoSoundGenerator : public ISoundGenerator
{
public:
	DECLARE_DELEGATE_TwoParams(FOnSoundInit, int32 , int32 );

	FCattleVideoSoundGenerator();

	// Called when a new buffer is required.
	virtual int32 OnGenerateAudio(float* OutAudio, int32 NumSamples) override;

	// Returns the number of samples to render per callback
	virtual int32 GetDesiredNumSamplesToRenderPerCallback() const;

	// Optional. Called on audio generator thread right when the generator begins generating.
	virtual void OnBeginGenerate() { bGeneratingAudio = true; };

	// Optional. Called on audio generator thread right when the generator ends generating.
	virtual void OnEndGenerate() { bGeneratingAudio = false; };

	// Optional. Can be overridden to end the sound when generating is finished.
	virtual bool IsFinished() const { return false; };

	void AddAudio(const float* AudioData, int InSampleRate, size_t NChannels, size_t NFrames);
	void Slient(bool S);
	void Volume(unsigned int value = 100);
	void StartAudio(int InSampleRate, size_t NChannels);

	int32 GetSampleRate() { return Params.SampleRate; }
	int32 GetNumChannels() { return Params.NumChannels; }
	void EmptyBuffers();
	void SetParameters(const FSoundGeneratorInitParams& InitParams);
	void SetParameters(int32 InSampleRate, int32 InNumChannels);
	FOnSoundInit& OnSoundInit();
private:
	FSoundGeneratorInitParams Params;
	TArray<float> Buffer;
	TQueue<TArray<float>> QBuffer;
	FCriticalSection CriticalSection;
	int32 SampleRate;
	int32 Channels;
	//uint32 volume;
	std::atomic<uint32> volume;
	volatile bool silent;
private:
	FThreadSafeBool bGeneratingAudio = false;
	//FThreadSafeBool bShouldGenerateAudio = false;

	FOnSoundInit _OnSoundInit;

};

