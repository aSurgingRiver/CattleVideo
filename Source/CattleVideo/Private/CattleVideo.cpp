// Fill out your copyright notice in the Description page of Project Settings.


#include "CattleVideo.h"
#include "Widgets/SViewport.h"
#include "CattleVideoViewport.h"
#include "CattleVideoControl.h"
#include "CattleVideoOption.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Text/STextBlock.h"

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
	Viewport = nullptr;
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
	TSharedPtr<SViewport> viewportS;
	auto Widget = SNew(SOverlay)
		+ SOverlay::Slot()
		.VAlign(VAlign_Fill)
		.HAlign(HAlign_Fill)
		[
			SAssignNew(viewportS,SViewport)
			.EnableGammaCorrection(false)
			.EnableBlending(true)
			.IgnoreTextureAlpha(false)
		];
	Viewport = MakeShared<FCattleVideoViewport>();
	Viewport->SetControl(Control);
	viewportS->SetViewportInterface(Viewport.ToSharedRef());
	return Widget;
}

bool UCattleVideo::Open(FString URL, FCattleVideoOption Option) {
	return Control ? Control->Open(URL, Option) : false;
}

void UCattleVideo::Close() {
	if (Control)Control->Close();
}

FCattleVideoOption UCattleVideo::DefaultOption() {
	FCattleVideoOption Opt;
	Opt.PkgCacheNumber = 30;
	Opt.Repeat = false;
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
	Viewport = nullptr;
	Control = nullptr;
}

void UCattleVideo::CopyFrom(UCattleVideo* From) {
	Control = From->Control;
	if (Viewport)Viewport->SetControl(Control);
}


#undef LOCTEXT_NAMESPACE

