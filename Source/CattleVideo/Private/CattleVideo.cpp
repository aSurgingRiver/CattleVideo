// Fill out your copyright notice in the Description page of Project Settings.


#include "CattleVideo.h"
#include "Widgets/SViewport.h"
#include "CattleVideoWidget.h"
#include "CattleVideoControl.h"
#include "CattleVideoOption.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Text/STextBlock.h"
#include "Engine/World.h"
#include "CattleVideoActor.h"
#define LOCTEXT_NAMESPACE "CattleVideo"

//
//UCattleVideoCopy::UCattleVideoCopy(const FObjectInitializer& ObjectInitializer)
//	: Super(ObjectInitializer)
//{
//	_Control = nullptr;
//}
//
//std::shared_ptr<UCattleVideoControl>& UCattleVideoCopy::Control() {
//	return _Control;
//}

UCattleVideo::UCattleVideo(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	Control = cattlevideo::GetControl();
	SoundActor = nullptr;
	//PkgCacheNumber = 30;
	//Repeat = false;
	//FFmpegDictionary.Add("timeout", "5000000");
}

TSharedRef<SWidget> UCattleVideo::RebuildWidget() {
	if (IsDesignTime() || IsDefaultSubobject()) {
		return SNew(SBox)
			.HAlign(HAlign_Center)
			.VAlign(VAlign_Center)
			[
				SNew(STextBlock)
				.Text(LOCTEXT("CattleVideo", "CattleVideo"))
			];
	}
	auto Overlay = SNew(SOverlay)
		+ SOverlay::Slot()
		.VAlign(VAlign_Fill)
		.HAlign(HAlign_Fill)
		[
			SAssignNew(Widget, SCattleVideoWidget)
		];
	GetSoundActor();
	BindHandle();
	return Overlay;
}

bool UCattleVideo::Open(FString URL, FCattleVideoOption Option) {
	GetSoundActor()->Slient(Option.Silent);
	return Control ? Control->Open(URL, Option) : false;
}

void UCattleVideo::Close() {
	if (Control)Control->Close();
}

void UCattleVideo::Slient(bool S) {
	GetSoundActor()->Slient(S);
}

FCattleVideoOption UCattleVideo::DefaultOption() {
	FCattleVideoOption Opt;
	Opt.PkgCacheNumber = 30;
	Opt.Repeat = false;
	Opt.Silent = false;
	/**
	*  stimeout 5000000 链接超时时间
	*  rtsp_transport tcp/udp  设置rtsp链接方式
	*  buffer_size 90000000  设置接收buffer大小
	*  fifo_size 90000000  设置接收buffer大小
	**/
	Opt.FFmpegDictionary.Add("stimeout", "10000000");//链接时间 单位微秒
	Opt.FFmpegDictionary.Add("buffer_size", "90000000");//链接时间 单位微秒
	//Opt.FFmpegDictionary.Add("fifo_size", "90000000");//链接时间 单位微秒
	//Opt.FFmpegDictionary.Add("rtsp_transport", "tcp");// 单位微秒 tcp/udp
	//Opt.FFmpegDictionary.Add("rtsp_transport", "udp");// 单位微秒

	return Opt;
}

void UCattleVideo::BeginDestroy() {
	Super::BeginDestroy();
	if (load_status_handle.IsValid()&& Control.IsValid()) {
		Control->OnLoadStatus().Remove(load_status_handle);
	}
	Control = nullptr;
}

void UCattleVideo::CopyFrom(UCattleVideo* From) {
	UnBindHandle();
	Control = From->Control;
	BindHandle();
	if(Widget)Widget->UpdateTexture( Control->GetTexture2D());
	SoundActor = From->GetSoundActor();
}

void UCattleVideo::Pause() {
	if (Control)Control->Pause();
}

void UCattleVideo::Resume() {
	if (Control)Control->Resume();
}

void UCattleVideo::Volume(int value) {
	value = FMath::Clamp(value,0,500);
	GetSoundActor()->Volume(value);
}

void UCattleVideo::Speed(CATTLE_VIDEO_SPEED value) {
	uint32 speed;
	switch (value) {
		case	CATTLE_VIDEO_SPEED::CATTLE_VIDEO_SPEED_050: speed = 50; break;
		case	CATTLE_VIDEO_SPEED::CATTLE_VIDEO_SPEED_075: speed = 75; break;
		case	CATTLE_VIDEO_SPEED::CATTLE_VIDEO_SPEED_100: speed = 100; break;
		case	CATTLE_VIDEO_SPEED::CATTLE_VIDEO_SPEED_125: speed = 125; break;
		case	CATTLE_VIDEO_SPEED::CATTLE_VIDEO_SPEED_150: speed = 150; break;
		case	CATTLE_VIDEO_SPEED::CATTLE_VIDEO_SPEED_175: speed = 175; break;
		case	CATTLE_VIDEO_SPEED::CATTLE_VIDEO_SPEED_200: speed = 200; break;
		case	CATTLE_VIDEO_SPEED::CATTLE_VIDEO_SPEED_225: speed = 225; break;
		case	CATTLE_VIDEO_SPEED::CATTLE_VIDEO_SPEED_250: speed = 250; break;
		case	CATTLE_VIDEO_SPEED::CATTLE_VIDEO_SPEED_300: speed = 300; break;
		default: speed = 100;
	}
	if (Control)Control->Speed(speed);
}

int UCattleVideo::Duration() const {
	return (Control)?Control->Duration():0;
}

bool UCattleVideo::Seek(int ms) {
	return (Control) ? Control->Seek(ms) : false;
}

void UCattleVideo::OnLoadStatusH(FString URL, int status, FString Desc) {
	if (OnLoadStatus.IsBound())	OnLoadStatus.Broadcast(URL,status, Desc);
}
void UCattleVideo::OnTextureSyncH(UTexture2D* NEW, UTexture2D* OLD) {
	if(Widget)Widget->UpdateTexture(NEW);// 可不添加到视口
	if (OnTextureSync.IsBound())OnTextureSync.Broadcast(NEW, OLD);
}
void UCattleVideo::BindHandle() {
	if (!Control.IsValid())return;
	load_status_handle = Control->OnLoadStatus().AddUObject(this, &UCattleVideo::OnLoadStatusH);
	texture_sync_handle = Control->OnTextureSync().AddUObject(this, &UCattleVideo::OnTextureSyncH);
}
void UCattleVideo::UnBindHandle() {
	if (!Control.IsValid())return;
	if (load_status_handle.IsValid()) Control->OnLoadStatus().Remove(load_status_handle);
	if (texture_sync_handle.IsValid()) Control->OnTextureSync().Remove(texture_sync_handle);
}

ACattleVideoActor* UCattleVideo::GetSoundActor() {
	if (SoundActor)return SoundActor;
	SoundActor = GetWorld()->SpawnActor<ACattleVideoActor>();
	SoundActor->SetContorl(Control);
	return SoundActor;
}

UTexture2D* UCattleVideo::Texture2D() {
	if (!Control.IsValid())return nullptr;
	return Control->GetTexture2D();
}

#undef LOCTEXT_NAMESPACE

