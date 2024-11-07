// Copyright Epic Games, Inc. All Rights Reserved.

#include "CattleVideoCore.h"
#include "CattleVideoLog.h"
#include "FFmpegLib.h"
#include "ffmpeg.hpp"
#include "CattleVideoControl.h"
#include "IFFmpgeLoad.h"
#define LOCTEXT_NAMESPACE "FCattleVideoCoreModule"

DEFINE_LOG_CATEGORY(CattleVideoLog);


namespace cattlevideo {
	void FFmpegLog(void* ptr, int level, const char* fmt, va_list vl);
}

void FCattleVideoCoreModule::StartupModule()
{
	IFFmpgeLoad::get();
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	//IFFmpegLIB::get()->LoadCEF3Modules();
	FFmpegCall(av_log_set_level)(AV_LOG_WARNING);// AV_LOG_VERBOSE AV_LOG_INFO AV_LOG_DEBUG
	FFmpegCall(av_log_set_callback)(cattlevideo::FFmpegLog);
	//avdevice_register_all();
	//av_regisger_all();
	//avcodec_register_all();
	FFmpegCall(avdevice_register_all)();
	FFmpegCall(avformat_network_init)();
}

void FCattleVideoCoreModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
	cattlevideo::OnEndPIE(false);
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FCattleVideoCoreModule, CattleVideoCore)