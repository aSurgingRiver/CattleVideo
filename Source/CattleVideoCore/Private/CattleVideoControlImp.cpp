// Fill out your copyright notice in the Description page of Project Settings.


#include "CattleVideoControlImp.h"
#include "Slate/SlateTextures.h"
#include "CattleVideoLog.h"
#include "CattleVideoOption.h"
#include "Async/Async.h"
#include "CattleConfig.h"
#include <thread>
#include <mutex>
#include <chrono>
#include "CattleVideoSoundGenerator.h"


namespace cattlevideo {

	//av_log_set_callback();
	void FFmpegLog(void* ptr, int level, const char* fmt, va_list vl) {
		if (FFmpegCall(av_log_get_level)() < level)return;
		char szLine[8192];	
		//snprintf(szLine,sizeof(szLine),"%s", fmt);
		vsnprintf(szLine, sizeof(szLine), fmt, vl);
		FString Line = UTF8_TO_TCHAR(szLine);
		Line = Line.Replace(TEXT("\n"),TEXT(""));
		switch (level&0xFF)
		{
		case AV_LOG_TRACE:
		case AV_LOG_DEBUG:
		case AV_LOG_VERBOSE:
		case AV_LOG_INFO:
			UE_LOG(CattleVideoLog, Log, TEXT("%s"), *Line);
			break;
		case AV_LOG_WARNING:
			UE_LOG(CattleVideoLog, Warning, TEXT("%s"), *Line);
			break;
		case AV_LOG_ERROR:
			UE_LOG(CattleVideoLog, Error, TEXT("%s"), *Line);
			break;
		case AV_LOG_FATAL:
		case AV_LOG_PANIC:
			UE_LOG(CattleVideoLog, Fatal, TEXT("%s"), *Line);
			break;
		}
	}

	class ControlMgr {
	public:
		void Registered(TWeakPtr<UCattleVideoControl, ESPMode::ThreadSafe> C) {
			//std::scoped_lock lock(m);
			m.lock();
			if (!mControl.Contains(C.Pin().Get())) {
				mControl.Add(C.Pin().Get(),C);
			}
			m.unlock();
		}
		void UnRegistered(TWeakPtr<UCattleVideoControl, ESPMode::ThreadSafe> C) {
			m.lock();
			if (mControl.Contains(C.Pin().Get())) {
				mControl.Remove(C.Pin().Get());
			}
			m.unlock();
		}
		void Close() {
			TMap<UCattleVideoControl*, TWeakPtr<UCattleVideoControl, ESPMode::ThreadSafe>> Copy;
			{m.lock();
			for (auto& it : mControl) {
				TWeakPtr<UCattleVideoControl, ESPMode::ThreadSafe> a = it.Value;
				if (!a.IsValid())continue;
				Copy.Add(a.Pin().Get(), a);
			}
			m.unlock();
			}
			for (auto it : Copy) {
				if (it.Value.IsValid())it.Value.Pin()->Close();
			}
		}
	private:
		std::mutex m;
		TMap<const UCattleVideoControl*, TWeakPtr<UCattleVideoControl, ESPMode::ThreadSafe>> mControl;
	};
	ControlMgr g_ControlMgr;


	CATTLEVIDEOCORE_API TSharedPtr<UCattleVideoControl, ESPMode::ThreadSafe> GetControl() {
		TSharedPtr<UCattleVideoControlImp, ESPMode::ThreadSafe> Contorl = MakeShared<UCattleVideoControlImp, ESPMode::ThreadSafe>();
		Contorl->SetOwner(Contorl);
		cattlevideo::g_ControlMgr.Registered(Contorl);
		return Contorl;
	}
	CATTLEVIDEOCORE_API void OnBeginPIE(bool) {
		isPIE = true;
		UE_LOG(CattleVideoLog, Log, TEXT("OnBeginPIE"));
	}
	CATTLEVIDEOCORE_API void OnEndPIE(bool) {
		isPIE = false;
		g_ControlMgr.Close();
		UE_LOG(CattleVideoLog, Log, TEXT("OnEndPIE"));
	}

}

UCattleVideoControlImp::UCattleVideoControlImp() {
	Compose = std::make_shared<cattlevideo::CAntCompose>();
	This = nullptr;
	Read = nullptr;
	Buffer = nullptr;
	duration = -1;
	position = 0;
}

UCattleVideoControlImp::~UCattleVideoControlImp() {
	cattlevideo::g_ControlMgr.UnRegistered(This);
	Compose->SetOwner(nullptr);
	Compose = nullptr;
	//UE_LOG(CattleVideoLog, Log, TEXT("~UCattleVideoControlImp"));
	Close();
}

void UCattleVideoControlImp::Close() {
	if (!Read) return;
	Read->SetCompose(nullptr);
	Read->Kill();
	//AsyncTask(ENamedThreads::AnyThread, [Read = Read]() {
	//	std::this_thread::sleep_for(std::chrono::milliseconds(10000));
	//	if (!cattlevideo::isPIE)return;
	//	AsyncTask(ENamedThreads::GameThread, [Read]() {

	//		});
	//	});
	Read = nullptr;
	//UE_LOG(CattleVideoLog, Log, TEXT("UCattleVideoControlImp::Close"));
}

void UCattleVideoControlImp::SetOwner(TWeakPtr<UCattleVideoControlImp, ESPMode::ThreadSafe> T) {
	This = T;
	Compose->SetOwner(T);
}

void UCattleVideoControlImp::ReOpen() {
	AsyncTask(ENamedThreads::GameThread,[This=This, URL= URL]() {
		std::this_thread::sleep_for(std::chrono::milliseconds(20000));
		AsyncTask(ENamedThreads::GameThread, [This, URL]() {
			if (!cattlevideo::isPIE)return;
			if(This.IsValid())
				This.Pin()->Open(URL);
			});
		});
}

bool UCattleVideoControlImp::Open(FString url, FCattleVideoOption& option) {
	opt.PkgCacheNumber = FMath::Clamp(option.PkgCacheNumber,30,1000);
	opt.Repeat = option.Repeat;
	//opt.WaitTime = option.WaitTime;
	opt.Hardware = option.Hardware;
	opt.WaitTime = FMath::Clamp(option.WaitTime, 1, 1000)*1000000;
	opt.FFmpegDictionary = option.FFmpegDictionary;
	Compose->AutoSize(false);
	return Open(url);
}

void UCattleVideoControlImp::SetSound(TSharedPtr<FCattleVideoSoundGenerator, ESPMode::ThreadSafe> SoundGenerator) {
	Compose->SetSound(SoundGenerator);
}

bool UCattleVideoControlImp::Open(FString url) {
	url = url.Replace(TEXT(" "), TEXT("")).Replace(TEXT("\n"), TEXT(""));
	if (url.Len() == 0 || !Compose)return false;
	URL = url;
	Close();
	Read = std::make_shared<cattlevideo::FThreadRead>();
	Read->SetCompose(Compose);
	return Read->Open(URL, opt);
}

//void UCattleVideoControlImp::UpdateTexture(std::shared_ptr < cattlevideo::CFrameBuffer > B) {
//	if (Buffer == B) {
//		Buffer->ReleaseCache();
//		return;
//	}
//	if (B->Group() != Read->Group())return;
//	Buffer = B;
//}
FGuid UCattleVideoControlImp::Group() {
	return Read ? Read->Group() : FGuid();
}
std::shared_ptr<cattlevideo::FThread> UCattleVideoControlImp::SelfThread(cattlevideo::FThread* self) {
	if (self == Read.get()) {
		return Read;
	}
	return Read ? Read->ParseThread(self) : nullptr;
}

UCattleVideoControl::FOnLoadStatus& UCattleVideoControlImp::OnLoadStatus() {
	return _OnLoadStatus;
}

UCattleVideoControl::FOnTextureSync& UCattleVideoControlImp::OnTextureSync() {
	return _OnTextureSync;
}

void UCattleVideoControlImp::OnLoadStatusH(FString _URL, int status,FString Desc) {
	if (_OnLoadStatus.IsBound())_OnLoadStatus.Broadcast(_URL, status, Desc);
}

void UCattleVideoControlImp::OnTextureSyncH(UTexture2D* NEW, UTexture2D* OLD) {
	if (_OnTextureSync.IsBound())_OnTextureSync.Broadcast(NEW, OLD);
}

UTexture2D* UCattleVideoControlImp::GetTexture2D() {
	return Compose ? Compose->GetTexture2D() : nullptr;
}

void UCattleVideoControlImp::Pause() {
	if (!Read)return;
	Read->SetPause(true);
}
void UCattleVideoControlImp::Resume() {
	if (!Read)return;
	Read->SetPause(false);
}
void UCattleVideoControlImp::Speed(uint32 sp) {
	//if (Compose) Compose->Speed(sp);
	if (Read) Read->SetSpeed(sp);
}

void UCattleVideoControlImp::AddSize(std::shared_ptr<cattlevideo::VideoSize> size) {
	if (Compose) Compose->AddSize(size);
}

void UCattleVideoControlImp::DelSize(std::shared_ptr<cattlevideo::VideoSize> size) {
	if (Compose) Compose->DelSize(size);
}

int  UCattleVideoControlImp::Duration() {
	if (Compose)return Compose->Duration();
	return 0;
}

bool UCattleVideoControlImp::Seek(int pos) {
	if (Duration() <= pos)return false;
//#define IMPSEEK_FLAG_BACKWARD 1 ///< seek backward
//#define IMPSEEK_FLAG_BYTE     2 ///< seeking based on position in bytes
//#define IMPSEEK_FLAG_ANY      4 ///< seek to any frame, even non-keyframes
//#define IMPSEEK_FLAG_FRAME    8 ///< seeking based on frame number
	Read->AddCmd([read = Read.get(), pos]() {read->Seek(pos*1000, 0,true); });
	return true;
}

void UCattleVideoControlImp::ProgressSynchro(bool synchro) {

}

namespace cattlevideo {
	void CAntCompose::UpdateTexture(std::shared_ptr < cattlevideo::CFrameBuffer > NEW, std::shared_ptr<CFrameBuffer> OLD) {
		UTexture2D* oldTexture=nullptr;
		UTexture2D* newTexture = nullptr;
		//FThread* thread=nullptr;
		if (OLD) oldTexture = OLD->Texture2D();
		if (NEW) { newTexture = NEW->Texture2D(); /*thread = NEW->VideoDecode().get();*/ }
		//if (thread) thread->AddCmd([OLD]() {});// ÊÍ·ÅÀÏ¾ä±ú
		if (!Owner.IsValid() || NEW->Group() != Owner.Pin()->Group())return;
		Buffer = NEW;
		Owner.Pin()->OnTextureSyncH(newTexture, oldTexture);
	}

	void CAntCompose::Position(uint32_t p) {
		position = p;
	}
	void CAntCompose::SetOwner(TWeakPtr<UCattleVideoControlImp, ESPMode::ThreadSafe> T) {
		Owner = T;
	}

	void CAntCompose::SetSound(TSharedPtr<FCattleVideoSoundGenerator, ESPMode::ThreadSafe> _SoundGenerator) {
		SoundGenerator = _SoundGenerator;
	}
	void CAntCompose::ReOpen() {
		if (!Owner.IsValid())return;
		Owner.Pin()->ReOpen();
	}
	void CAntCompose::Duration(uint32_t d) {
		duration = d/1000;
	}

	UTexture2D* CAntCompose::GetTexture2D() {
		if (!Buffer)return nullptr;
		return Buffer->Texture2D();
	}

	std::shared_ptr<FThread> CAntCompose::SelfThread(cattlevideo::FThread* self) {
		if (!Owner.IsValid())return nullptr;
		return Owner.Pin()->SelfThread(self);
	}

	void CAntCompose::OnLoadStatus(FString URL, int status, FString Desc) {
		if (!Owner.IsValid())return;
		Owner.Pin()->OnLoadStatusH(URL, status, Desc);
	}
	uint32 CAntCompose::Speed() {
		return speed;
	}

	void CAntCompose::Speed(uint32 sp) {
		speed=sp;
	}

	CAntCompose::CAntCompose() {
		speed = 100;
		duration = 100;
		bAutoSize = false;
		position = 0;
		duration = 0;
	}

	CAntCompose::~CAntCompose() {
		Buffer = nullptr;
		Owner = nullptr;
	}

	void CAntCompose::StartAudio(int InSampleRate, size_t NChannels) {
		if (!SoundGenerator.IsValid()) return;
		SoundGenerator.Pin()->StartAudio(InSampleRate, NChannels);
	}

	void CAntCompose::AddAudio(const float* AudioData, int InSampleRate, size_t NChannels, size_t NFrames) {
		if (!SoundGenerator.IsValid()) return;
		SoundGenerator.Pin()->AddAudio(AudioData,InSampleRate,NChannels,NFrames);
	}
	bool CAntCompose::GetSize(int& width, int& height) {
		bool autoSize = bAutoSize;
		if (autoSize == false || nullptr==pre_size) {
			return false;
		}
		cattlevideo::VideoSize& s = *pre_size;
		int Twidth = s.width;
		int Theight = s.height;
		if (Twidth <= 1 || height <= 1)
			return false;
		width = FMath::Clamp(Twidth,16, width);
		height = FMath::Clamp(Theight,16, height);
		return true;
	}
	void CAntCompose::AddSize(std::shared_ptr<cattlevideo::VideoSize> size) {
		if (Sizes.Contains(size))return;
		Sizes.Add(size);
		pre_size = size;
	}
	void CAntCompose::DelSize(std::shared_ptr<cattlevideo::VideoSize> size) {
		if (!Sizes.Contains(size) || Sizes.Num() == 0)return;
		Sizes.Remove(size);
		if (size!=pre_size)return;
		pre_size = Sizes.Num() == 0 ? nullptr : Sizes[Sizes.Num() - 1];
	}
	void CAntCompose::AutoSize(bool autoSize) {
		bAutoSize = CCattleConfig::get()->AutoSize(autoSize);
	}

	int32 CAntCompose::Duration() {
		return duration;
	}
}