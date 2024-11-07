// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CattleVideoControl.h"
#include "CattleVideoData.h"
#include <map>
#include <atomic>
//#include "CattleVideoControlImp.generated.h"
class UCattleVideoControlImp;
class FCattleVideoSoundGenerator;
namespace cattlevideo {
	class CAntCompose : public AntCompose {
	public:
		CAntCompose();
		virtual ~CAntCompose();
		virtual void UpdateTexture(std::shared_ptr < cattlevideo::CFrameBuffer >, std::shared_ptr<CFrameBuffer>);
		virtual void Duration(uint32_t);
		virtual void Position(uint32_t);
		virtual void ReOpen();
		virtual void StartAudio(int InSampleRate, size_t NChannels);
		virtual void AddAudio(const float* AudioData, int InSampleRate, size_t NChannels, size_t NFrames);
		virtual bool GetSize(int&,int&);
		virtual std::shared_ptr<FThread> SelfThread(FThread*);
		virtual void OnLoadStatus(FString, int, FString Desc);
		virtual uint32 Speed();
		void SetOwner(TWeakPtr<UCattleVideoControlImp, ESPMode::ThreadSafe> T);
		void SetSound(TSharedPtr<FCattleVideoSoundGenerator, ESPMode::ThreadSafe> SoundGenerator);
		void AddSize(std::shared_ptr<cattlevideo::VideoSize> size);
		void DelSize(std::shared_ptr<cattlevideo::VideoSize> size);
		void AutoSize(bool autoSize);
		int32 Duration();
		virtual void Speed(uint32 sp);
		UTexture2D* GetTexture2D();
	private:
		std::shared_ptr<cattlevideo::VideoSize> pre_size;
		TArray<std::shared_ptr<cattlevideo::VideoSize>> Sizes;
		TWeakPtr<UCattleVideoControlImp, ESPMode::ThreadSafe> Owner;
		std::shared_ptr < cattlevideo::CFrameBuffer > Buffer;
		uint32_t duration;
		uint32_t position;
		std::atomic<bool> bAutoSize;
		std::atomic<uint32> speed;
		TWeakPtr<FCattleVideoSoundGenerator, ESPMode::ThreadSafe> SoundGenerator;
	};
}

/**
 * 
 */
//UCLASS()
class UCattleVideoControlImp : public UCattleVideoControl/*,public cattlevideo::AntCompose*//*,public TSharedFromThis<UCattleVideoControlImp, ESPMode::ThreadSafe>*/
{
	//GENERATED_UCLASS_BODY()
public:
	UCattleVideoControlImp();
	~UCattleVideoControlImp();
	//void RegisteredSkin(UObject* Skin);
	//virtual void UnregisteredSkin(UObject* Skin);
	virtual FOnLoadStatus& OnLoadStatus();
	virtual FOnTextureSync& OnTextureSync();
	virtual UTexture2D* GetTexture2D();
	virtual bool Open(FString URL, FCattleVideoOption& option);
	virtual void SetSound(TSharedPtr<FCattleVideoSoundGenerator, ESPMode::ThreadSafe> SoundGenerator);
	virtual void Pause();
	virtual void Resume();
	virtual void Speed(uint32);
	virtual void AddSize(std::shared_ptr<cattlevideo::VideoSize> size);
	virtual void DelSize(std::shared_ptr<cattlevideo::VideoSize> size);
	//virtual void UpdateTexture(std::shared_ptr < cattlevideo:: CFrameBuffer > );
	//virtual void Duration(uint32_t);
	//virtual void Position(uint32_t);
	virtual int  Duration();
	virtual bool Seek(int pos);
	virtual void ProgressSynchro(bool synchro);
	virtual void ReOpen();
	void SetOwner(TWeakPtr<UCattleVideoControlImp, ESPMode::ThreadSafe> T);
	void Close();
	FGuid Group();
	std::shared_ptr<cattlevideo::FThread> SelfThread(cattlevideo::FThread*);

	void OnLoadStatusH(FString, int, FString Desc);
	void OnTextureSyncH(UTexture2D*, UTexture2D*);
private:
	bool Open(FString URL);

private:
	FOnLoadStatus _OnLoadStatus;
	FOnTextureSync _OnTextureSync;
	cattlevideo::FThreadRead::Option opt;
	FString URL;
	std::shared_ptr<cattlevideo::FThreadRead> Read;
	std::shared_ptr < cattlevideo::CFrameBuffer > Buffer;
	uint32_t duration;
	uint32_t position;
	std::shared_ptr<cattlevideo::CAntCompose> Compose;
	TWeakPtr<UCattleVideoControlImp, ESPMode::ThreadSafe> This;
};
