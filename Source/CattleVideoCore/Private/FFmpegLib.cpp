
#include "FFmpegLib.h"
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
#include "Misc/Paths.h"
#include <string>
#include <stdlib.h>
// WEB_CORE_API

// WEB_CORE_API
class CFFmpegLIB : public IFFmpegLIB {
public:
	void LoadCEF3Modules() ;
	void UnloadCEF3Modules() ;
	FString LibPath() ;
	virtual ~CFFmpegLIB() = default;
private:
	void* LoadDllCEF(const FString& Path);
private:
	std::vector<void*> dllHand;

#if PLATFORM_MAC
	CefScopedLibraryLoader CEFLibraryLoader;
#endif
};

IFFmpegLIB* IFFmpegLIB::get() {
	static IFFmpegLIB* install= nullptr;
	//UE_LOG(CoreWebLog, Error, TEXT("CEF3DLL::get"));
	if (nullptr == install) {
		install = new CFFmpegLIB();
	}
	return install;
}

void* CFFmpegLIB::LoadDllCEF(const FString& Path)
{
	if (Path.IsEmpty())
	{
		return nullptr;
	}
	void* Handle = FPlatformProcess::GetDllHandle(*Path);
	if (!Handle)
	{
		int32 ErrorNum = FPlatformMisc::GetLastError();
		TCHAR ErrorMsg[1024];
		FPlatformMisc::GetSystemErrorMessage(ErrorMsg, 1024, ErrorNum);
		UE_LOG(LogTemp, Error, TEXT("Failed to get CEF3 DLL handle for %s: %s (%d)"), *Path, ErrorMsg, ErrorNum);
	}
	else {
		dllHand.push_back(Handle);
	}
	return Handle;
}

FString CFFmpegLIB::LibPath() {
	TSharedPtr<IPlugin> Plugin = IPluginManager::Get().FindPlugin(TEXT("AntVedio"));
	if (!Plugin.IsValid()) {
		Plugin = IPluginManager::Get().FindPlugin(TEXT("WebView"));
	}
	const FString BaseDir = FPaths::ConvertRelativePathToFull(Plugin->GetBaseDir());
	FString Version = FString::Printf(TEXT("%d.%d"), FFMPEG_MAJOR, FFMPEG_MINOR);
	FString LibPath;
#if PLATFORM_WINDOWS
	LibPath = FPaths::Combine(*BaseDir, TEXT("Source/ThirdParty/FFmpegForUe"), *Version, TEXT("win64/lib"));
#elif PLATFORM_MAC
	LibPath = FPaths::Combine(*BaseDir, TEXT("Source/ThirdParty/FFmpegForUe"), *Version, TEXT("mac/lib"));
#elif PLATFORM_LINUX
	LibPath = FPaths::Combine(*BaseDir, TEXT("Source/ThirdParty/FFmpegForUe"), *Version, TEXT("linux/lib"));
#endif
	return LibPath;
}

void CFFmpegLIB::LoadCEF3Modules()
{
	if (dllHand.size())return;// has load
	//UE_LOG(CoreWebLog, Error, TEXT("CEF3DLL::LoadCEF3Modules"));
	FString libPath = LibPath();
#if PLATFORM_WINDOWS
	FString envPath = FPlatformMisc::GetEnvironmentVariable(TEXT("Path")) + TEXT(";") + libPath;
	FPlatformMisc::SetEnvironmentVar(TEXT("Path"), *envPath);
	FPlatformProcess::PushDllDirectory(*libPath);
	if (LoadDllCEF(FPaths::Combine(*libPath, TEXT("avcodec-59.dll")))) {
		LoadDllCEF(FPaths::Combine(*libPath, TEXT("avdevice-59.dll")));
		LoadDllCEF(FPaths::Combine(*libPath, TEXT("avfilter-8.dll")));
		LoadDllCEF(FPaths::Combine(*libPath, TEXT("avformat-59.dll")));
		LoadDllCEF(FPaths::Combine(*libPath, TEXT("avutil-57.dll")));
		LoadDllCEF(FPaths::Combine(*libPath, TEXT("postproc-56.dll")));
		LoadDllCEF(FPaths::Combine(*libPath, TEXT("swresample-4.dll")));
		LoadDllCEF(FPaths::Combine(*libPath, TEXT("swscale-6.dll")));
	}
	FPlatformProcess::PopDllDirectory(*libPath);
#endif
}

void CFFmpegLIB::UnloadCEF3Modules()
{
	for (auto it = dllHand.rbegin(); it != dllHand.rend(); it++) {
		FPlatformProcess::FreeDllHandle(*it);
	}
	dllHand.clear();
}


