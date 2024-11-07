// Fill out your copyright notice in the Description page of Project Settings.


#include "CattleVideoSoundGenerator.h"


/*
* ---------------- FWebRTCSoundGenerator -------------------------
*/

FCattleVideoSoundGenerator::FCattleVideoSoundGenerator()
	: Params()
	, Buffer()
{
	volume = 100;
}

void FCattleVideoSoundGenerator::SetParameters(const FSoundGeneratorInitParams& InitParams)
{
	Params = InitParams;
}

void FCattleVideoSoundGenerator::SetParameters(int32 InSampleRate, int32 InNumChannels) {
	Params.SampleRate = InSampleRate;
	Params.NumChannels = InNumChannels;
}

int32 FCattleVideoSoundGenerator::GetDesiredNumSamplesToRenderPerCallback() const
{
	return Params.NumFramesPerCallback * Params.NumChannels;
}

void FCattleVideoSoundGenerator::EmptyBuffers()
{
	//FScopeLock Lock(&CriticalSection);
	QBuffer.Empty();
}


void FCattleVideoSoundGenerator::StartAudio(int InSampleRate, size_t NChannels) {
	Channels = NChannels;
	SampleRate = InSampleRate;
	OnSoundInit().ExecuteIfBound(SampleRate,Channels);
}

void FCattleVideoSoundGenerator::Slient(bool S) {
	silent = S;
}

void FCattleVideoSoundGenerator::Volume(unsigned int value) {
	volume.store(value);
}

void FCattleVideoSoundGenerator::AddAudio(const float* AudioData, int InSampleRate, size_t NChannels, size_t NFrames)
{
	if (silent) return;
	int NSamples = NFrames * NChannels;
	TArray<float> Data(AudioData, NSamples);// (NSamples);
	if (volume != 100) {// 
		float fVolume = volume / 100.0f;
		for (auto& it : Data) {
			it *= fVolume;
		}
	}
	QBuffer.Enqueue(Data);
}

// Called when a new buffer is required.
int32 FCattleVideoSoundGenerator::OnGenerateAudio(float* OutAudio, int32 NumSamples)
{
	if (silent){
		Buffer.Empty();
		QBuffer.Empty();
		return NumSamples;//
	}
	int32 ReadNum = 0;
	int32 ArrayRead = 0;
	for (;ArrayRead<NumSamples; ArrayRead += ReadNum) {
		if (Buffer.Num() == 0 && !QBuffer.Dequeue(Buffer)) {
			break;
		}
		ReadNum = FGenericPlatformMath::Min(Buffer.Num(),NumSamples-ArrayRead);
		memcpy(OutAudio+ArrayRead,Buffer.GetData(),ReadNum*sizeof(float));
		if (ReadNum == Buffer.Num())Buffer.Empty();
		else Buffer.RemoveAt(0, ReadNum, false);
	}
	return ArrayRead ==0?NumSamples: ArrayRead;
}

FCattleVideoSoundGenerator::FOnSoundInit& FCattleVideoSoundGenerator::OnSoundInit() {
	return _OnSoundInit;
}
