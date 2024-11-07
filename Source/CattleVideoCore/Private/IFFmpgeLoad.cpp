// Fill out your copyright notice in the Description page of Project Settings.


#include "IFFmpgeLoad.h"
#include "GenericPlatform/GenericPlatformProcess.h"
#include "GenericPlatform/GenericPlatformMisc.h"
#if PLATFORM_WINDOWS
#include "Windows/WindowsPlatformProcess.h"
#include "Windows/WindowsPlatformMisc.h"
#elif PLATFORM_LINUX
#include "Linux/LinuxPlatformProcess.h"
#include "Linux/LinuxPlatformMisc.h"
#elif PLATFORM_MAC
#include "Mac/MacPlatformProcess.h"
#include "Mac/MacPlatformMisc.h"
#endif
#include "HAL/FileManager.h"
#include "CattleVideoLog.h"
#include "Interfaces/IPluginManager.h"
#include <map>
#include <string>

class CFFmpgeLoad : public IFFmpgeLoad {
public:
	struct LibInfo{
		LibInfo(std::string m,std::string n, void* l=nullptr) {
			module = UTF8_TO_TCHAR(m.c_str());
			Name = UTF8_TO_TCHAR(n.c_str());
			lib = l;
		}
		FString module;
		FString Name;
		void* lib;
		bool operator==(const FString& m)const {
			return module == m;
		}
	};

	CFFmpgeLoad();
	void InitLibName();
	FString LibPath();
	void* GetLib(std::string& module);
	void* GetFunc(std::string module, std::string name);
	void* LoadLib(FString path);
	void LoadFunc() ;
	void UnloadFunc() ;
	TArray<LibInfo> Libs;
};

bool operator==(const CFFmpgeLoad::LibInfo& info,const FString& m) {
	return info.module == m;
}

void* CFFmpgeLoad::LoadLib(FString name) {
	if (name.Len()==0)return nullptr;
	FString file_path = FPaths::Combine(*LibPath(), *name);
	void* Handle = FPlatformProcess::GetDllHandle(*file_path);
	if (!Handle)
	{
		int32 ErrorNum = FPlatformMisc::GetLastError();
		TCHAR ErrorMsg[1024];
		FPlatformMisc::GetSystemErrorMessage(ErrorMsg, 1024, ErrorNum);
		UE_LOG(CattleVideoLog, Error, TEXT("Failed to get ffmpeg lib handle for %s: %s (%d)"), *file_path, ErrorMsg, ErrorNum);
	}
	return Handle;
}

void* CFFmpgeLoad::GetLib(std::string& module) {
	FString m = UTF8_TO_TCHAR(module.c_str());
	for (auto& it : Libs) {
		if (it.module == m)return it.lib;
	}
	//if (Libs.Contains(module))return Libs.FindByKey(m)->lib;
	return nullptr;
}

void* CFFmpgeLoad::GetFunc(std::string module, std::string name) {
	FString Name = UTF8_TO_TCHAR(name.c_str());
	void* m = GetLib(module);
	if (nullptr == m) {
		UE_LOG(CattleVideoLog, Error, TEXT("GetFunc module:%s = nullptr function:%s load failed!"), UTF8_TO_TCHAR(module.c_str()), *Name);
		return nullptr;
	}
	void* func = FPlatformProcess::GetDllExport(m, *Name);
	if (func == nullptr) {
		UE_LOG(CattleVideoLog, Error, TEXT("GetFunc module:%s function:%s = nullptr load failed!"), UTF8_TO_TCHAR(module.c_str()), *Name);
		return nullptr;
	}
	return func;
}

FString CFFmpgeLoad::LibPath() {
	TSharedPtr<IPlugin> Plugin = IPluginManager::Get().FindPlugin(TEXT("CattleVideo"));
	const FString BaseDir = FPaths::ConvertRelativePathToFull(Plugin->GetBaseDir());
	FString LibPath;
	FString Version = FString::Printf(TEXT("%d.%d"), FFMPEG_MAJOR, FFMPEG_MINOR);
#if PLATFORM_WINDOWS
	LibPath = FPaths::Combine(*BaseDir, TEXT("Source/ThirdParty/FFmpegForUe"), *Version, TEXT("Win64/lib"));
#elif PLATFORM_MAC
	LibPath = FPaths::Combine(*BaseDir, TEXT("Source/ThirdParty/FFmpegForUe"), *Version, TEXT("mac/lib"));
#elif PLATFORM_LINUX
	LibPath = FPaths::Combine(*BaseDir, TEXT("Source/ThirdParty/FFmpegForUe"), *Version, TEXT("Linux/lib"));
#elif PLATFORM_ANDROID
	LibPath = FPaths::Combine(*BaseDir, TEXT("Source/ThirdParty/FFmpegForUe"), *Version, TEXT("Android/lib"));
#endif
	return LibPath;
}

void CFFmpgeLoad::InitLibName() {
	FString envName;
#if PLATFORM_WINDOWS
	envName = TEXT("Path");
#   if FFMPEG_MAJOR==6
	Libs.Add({ "avutil","avutil-58.dll" });
	Libs.Add({ "swresample","swresample-4.dll" });
	Libs.Add({ "swscale","swscale-7.dll" });
	Libs.Add({ "postproc","postproc-57.dll" });
	Libs.Add({ "avcodec","avcodec-60.dll" });
	Libs.Add({ "avformat","avformat-60.dll" });
	Libs.Add({ "avfilter","avfilter-9.dll" });
	Libs.Add({ "avdevice","avdevice-60.dll" });
#   else if FFMPEG_MAJOR==4
	Libs.Add({ "avutil","avutil-56.dll" });
	Libs.Add({ "swresample","swresample-3.dll" });
	Libs.Add({ "swscale","swscale-5.dll" });
	Libs.Add({ "avcodec","avcodec-58.dll" });
	Libs.Add({ "avformat","avformat-58.dll" });
	Libs.Add({ "avfilter","avfilter-7.dll" });
	Libs.Add({ "avdevice","avdevice-58.dll" });
#   endif
#elif PLATFORM_MAC
	envName = TEXT("LD_LIBRARY_PATH");
	Libs.Add({ "avutil","libavutil.so" });
	Libs.Add({ "swresample","libswresample.so" });
	Libs.Add({ "swscale","libswscale.so" });
	Libs.Add({ "avcodec","libavcodec.so" });
	Libs.Add({ "avformat","libavformat.so" });
	Libs.Add({ "avfilter","libavfilter.so" });
	Libs.Add({ "avdevice","libavdevice.so" });
#elif PLATFORM_LINUX
	envName = TEXT("LD_LIBRARY_PATH");
	Libs.Add({ "libcrypto","libcrypto.so.1.1" });
	Libs.Add({ "libssl","libssl.so.1.1" });
	Libs.Add({ "avutil","libavutil.so" });
	Libs.Add({ "swresample","libswresample.so" });
	Libs.Add({ "swscale","libswscale.so" }); 
	Libs.Add({ "avcodec","libavcodec.so" });
	Libs.Add({ "avformat","libavformat.so" });
	Libs.Add({ "avfilter","libavfilter.so" });
	Libs.Add({ "avdevice","libavdevice.so" });
#elif PLATFORM_ANDROID
	//Libs.Add({ "avutil","libavutil.so" });
	//Libs.Add({ "swresample","libswresample.so" });
	//Libs.Add({ "swscale","libswscale.so" });
	//Libs.Add({ "avcodec","libavcodec.so" });
	//Libs.Add({ "avformat","libavformat.so" });
	//Libs.Add({ "avfilter","libavfilter.so" });
	//Libs.Add({ "avdevice","libavdevice.so" });
#endif
	FString libPath = LibPath();
	FString envPath = FPlatformMisc::GetEnvironmentVariable(*envName) + TEXT(";") + libPath;
	FPlatformMisc::SetEnvironmentVar(*envName, *envPath);
	FPlatformProcess::PushDllDirectory(*libPath);
	for (auto& it : Libs) {
		it.lib = LoadLib(it.Name);
	}
	FPlatformProcess::PopDllDirectory(*libPath);
}

CFFmpgeLoad::CFFmpgeLoad() {
	InitLibName();
	LoadFunc();
}

void CFFmpgeLoad::LoadFunc() {
#if !PLATFORM_ANDROID
#define GetMoudleFunc(module,name) \
	cattlevideo::v_##name = (f_##name)GetFunc(#module,#name)
#else
#define GetMoudleFunc(module,name) 
#endif

	GetMoudleFunc(avutil, av_strerror);
	GetMoudleFunc(avutil, av_frame_unref);
	GetMoudleFunc(avutil, av_frame_free);
	GetMoudleFunc(avutil, av_frame_alloc);
	GetMoudleFunc(avutil, av_frame_clone);
	GetMoudleFunc(avutil, av_gettime_relative);
	GetMoudleFunc(avutil, av_hwframe_transfer_data);
	GetMoudleFunc(avutil, av_hwdevice_ctx_create);
	GetMoudleFunc(avutil, av_dict_free);
	GetMoudleFunc(avutil, av_dict_set);
	GetMoudleFunc(avutil, av_dict_get);
#if FFMPEG_MAJOR==6
	GetMoudleFunc(avutil, av_channel_layout_copy);
	GetMoudleFunc(avutil, av_channel_layout_compare);
#else 
	cattlevideo::v_av_channel_layout_copy = av_channel_layout_copy;
	cattlevideo::v_av_channel_layout_compare = av_channel_layout_compare;
#endif
	GetMoudleFunc(avutil, av_samples_alloc_array_and_samples);
	GetMoudleFunc(avutil, av_freep);
	GetMoudleFunc(avutil, av_samples_alloc);
	GetMoudleFunc(avutil, av_image_fill_arrays);
	GetMoudleFunc(avutil, av_log_set_level);
	GetMoudleFunc(avutil, av_log_set_callback);
	GetMoudleFunc(avutil, av_log_get_level);
	GetMoudleFunc(avcodec, avcodec_receive_frame);
	GetMoudleFunc(avcodec, avcodec_send_packet);
	GetMoudleFunc(avcodec, av_packet_unref);
	GetMoudleFunc(avcodec, av_packet_ref);
	GetMoudleFunc(avcodec, av_packet_free);
	GetMoudleFunc(avcodec, av_packet_alloc);
	GetMoudleFunc(avcodec, av_packet_clone);
	GetMoudleFunc(avcodec, avcodec_find_decoder);
	GetMoudleFunc(avcodec, avcodec_alloc_context3);
	GetMoudleFunc(avcodec, avcodec_free_context);
	GetMoudleFunc(avcodec, avcodec_parameters_to_context);
	GetMoudleFunc(avcodec, avcodec_get_hw_config);
	GetMoudleFunc(avcodec, avcodec_open2);
	GetMoudleFunc(avformat, avformat_alloc_context);
	GetMoudleFunc(avformat, avformat_open_input);
	GetMoudleFunc(avformat, avformat_close_input);
	GetMoudleFunc(avformat, av_format_inject_global_side_data);
	GetMoudleFunc(avformat, av_find_best_stream);
	GetMoudleFunc(avformat, avformat_find_stream_info);
	GetMoudleFunc(avformat, av_dump_format);
	GetMoudleFunc(avformat, avformat_seek_file);
	GetMoudleFunc(avformat, avformat_network_init);
	GetMoudleFunc(avformat, av_read_frame);
	GetMoudleFunc(swresample, swr_alloc);
	GetMoudleFunc(swresample, swr_init);
	GetMoudleFunc(swresample, swr_free);
#if FFMPEG_MAJOR==6
	GetMoudleFunc(swresample, swr_alloc_set_opts2);
#else 
	cattlevideo::v_swr_alloc_set_opts2 = swr_alloc_set_opts2;
#endif
	GetMoudleFunc(swresample, swr_alloc_set_opts);
	GetMoudleFunc(swresample, swr_convert);
	GetMoudleFunc(swscale, sws_getCachedContext);
	GetMoudleFunc(swscale, sws_freeContext);
	GetMoudleFunc(swscale, sws_scale);
	GetMoudleFunc(avdevice, avdevice_register_all);
	UE_LOG(CattleVideoLog, Log, TEXT("FFmpeg lib load finish!"));
}

void CFFmpgeLoad::UnloadFunc() {
	for (int id = Libs.Num()-1; id; id--) {
		FPlatformProcess::FreeDllHandle(Libs[id].lib);
	}
	Libs.Empty();
}

IFFmpgeLoad* IFFmpgeLoad::get() {
	static CFFmpgeLoad* load=nullptr;
	if (nullptr == load) {
		load = new CFFmpgeLoad;
	}
	return load;
}
