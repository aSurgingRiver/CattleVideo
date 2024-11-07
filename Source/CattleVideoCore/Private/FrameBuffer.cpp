
#include "FrameBuffer.h"
#include "Slate/SlateTextures.h"
#include "CattleVideoLog.h"
#include "Rendering/SlateRenderer.h"
#include "Framework/Application/SlateApplication.h"
#include "Slate/SlateTextures.h"
#include "CattleVideoData.h"
#include <vector>
#include "Engine/Texture2D.h"
//#include "Launch/Resources/Version.h"
#if defined WITH_OPENCV && WITH_OPENCV
#include "PreOpenCVHeaders.h"
#include "opencv2/opencv.hpp"
#include "PostOpenCVHeaders.h"
#endif

namespace cattlevideo {

	CATTLEVIDEOCORE_API void WritePic(int Height, int Width, void* data, FString prefix,int index) {
#if defined WITH_OPENCV && WITH_OPENCV
		FString prj = FPaths::Combine(*FPaths::ProjectDir(), "picture");
		if (!IFileManager::Get().DirectoryExists(*prj)) {
			IFileManager::Get().MakeDirectory(*prj, true);
		}
		FDateTime time = FDateTime::Now();
		FString file = FString::Printf(TEXT("%s_%s_%d.png"), *prefix, *time.ToString(), index);
		file = FPaths::Combine(*prj, *file);
		std::string saveF = TCHAR_TO_UTF8(*file);

		cv::Mat src(Height, Width, CV_8UC4, data);
		cv::imwrite(saveF.c_str(), src);
#endif
	}

	FString ErrorString(int error);
	bool IsSuccessed(int error, FString& Desc);

	CFrameBuffer::CFrameBuffer(AVPixelFormat src, int w, int h)
		: /*width(w)
		, height(h)
		,*/ srcFormat(src)
		, decodeData(nullptr,w,h,4)
	{
		Format = PF_Unknown;
		swsContext = nullptr;
		_Texture2D = nullptr;
		CacheDelay = 8;
		cache_buffer = nullptr;
		//preBuff = nullptr;
	}

	CFrameBuffer::~CFrameBuffer() {
		ReleaseSws();
		if (_Texture2D)_Texture2D->RemoveFromRoot();
	}

	bool CFrameBuffer::Compare(AVPixelFormat src, int w, int h) {
		return srcFormat==src && decodeData.GetWidth() == w && decodeData.GetHeight() == h;
	}

	//void CFrameBuffer::SetCache(std::shared_ptr<CFrameBuffer> R) {
	//	if(R) R->ReleaseSws();
	//	cache_buffer = R;
	//}
	//std::shared_ptr<CFrameBuffer>& CFrameBuffer::CacheBuffer() {
	//	return cache_buffer;
	//}
	void CFrameBuffer::ClearDecodeData() {
		auto& Bytes = decodeData.GetRawBytes();
		uint32_t color = 0xFF000000;
		std::vector<uint32_t> vColor(decodeData.GetHeight()*decodeData.GetWidth(), color);
		memcpy(decodeData.GetRawBytesPtr(), &vColor[0], Bytes.Num());
	}

	void CFrameBuffer::ReleaseCache() {
		if (!cache_buffer)return;
		CacheDelay--;
		if (0 < CacheDelay)return;
		cache_buffer = nullptr;
	}

	FGuid& CFrameBuffer::Group() {
		return group;
	}

	//FSlateUpdatableTexture* CFrameBuffer::Texture() {
	//	return _Texture;
	//}
	UTexture2D* CFrameBuffer::Texture2D() {
		return _Texture2D;
	}
	AVPixelFormat CFrameBuffer::TransFormat(AVPixelFormat src) {
		switch (src) {
		case AV_PIX_FMT_NV12:Format = PF_NV12; break;
		case AV_PIX_FMT_ARGB:Format = PF_A8R8G8B8; break;
		case AV_PIX_FMT_RGBA:Format = PF_R8G8B8A8; break;
		case AV_PIX_FMT_ABGR:Format = PF_NV12; break;
		default:Format = PF_B8G8R8A8; src = AV_PIX_FMT_BGRA; break;
		}
		return src;
	}

	bool CFrameBuffer::InitSws(AVPixelFormat dst, int w, int h) {
		const int width = decodeData.GetWidth();
		const int height = decodeData.GetHeight();
		if (srcFormat == dst) // xiang deng , zhi kao bei 
			return true;
		//int decodeLen = decodeData.GetRawBytes().Num();
		//size_t byte_buffer_size = av_image_get_buffer_size(dst, width, height, 1);
		//midData.resize(byte_buffer_size);
		// SWS_BICUBIC
		swsContext = FFmpegCall(sws_getCachedContext)(swsContext, w, h, (AVPixelFormat)srcFormat,
			width, height, dst, SWS_BICUBIC, NULL, NULL, NULL);
		if (swsContext == nullptr)
			return false;
		//int re = av_image_fill_arrays(pixels, pitch, midData.data(), dst, width, height, 1);
		//size_t byte_buffer_size = av_image_get_buffer_size(dst, width, height, 1);
		int re = FFmpegCall(av_image_fill_arrays)(pixels, pitch, decodeData.GetRawBytesPtr(), dst, width, height, 1);
		FString Desc;
		if (!IsSuccessed(re, Desc)) {
			return false;
		}
		Format = PF_B8G8R8A8;// PF_NV12 PF_B8G8R8A8
		return true;
	}
	void CFrameBuffer::ReleaseSws() {
		if (swsContext)
			FFmpegCall(sws_freeContext)(swsContext);
		swsContext = nullptr;
	}

	bool CFrameBuffer::Copy(AVFrame* F) {
		//UE_LOG(CattleVideoLog, Log, TEXT("CFrameBuffer::Copy "));
		if (!swsContext) {
			memcpy(decodeData.GetRawBytesPtr(), F->data[0], decodeData.GetWidth()* decodeData.GetHeight()*4);
			return true;
		}
		//uint8_t* pData = midData.data();
		//auto pDst = decodeData.GetRawBytesPtr();
		//memset(pData,0, midData.size());
		int result = FFmpegCall(sws_scale)(swsContext, (const uint8_t* const*)F->data, F->linesize,
			0, F->height, pixels, pitch);
		//memcpy(pDst, pData, midData.size());
		//WritePic(decodeData.GetHeight(), decodeData.GetWidth(), pData,TEXT("Copy"));
		return true;
	}
	bool Copy(AVFrame* src, std::shared_ptr<CFrameBuffer>& cach, FVedioDecode* thread) {
		bool isChang = false;
		int width = src->width;
		int height = src->height;
		thread->GetCompose()->GetSize(width,height);
		if (!(cach && cach->Compare((AVPixelFormat)src->format, width,height))) {
			std::shared_ptr<CFrameBuffer> dst;
			dst = std::make_shared<CFrameBuffer>((AVPixelFormat)src->format, width, height);
			if (!dst->InitSws(AV_PIX_FMT_BGRA,src->width,src->height))return false;//AV_PIX_FMT_NV12 AV_PIX_FMT_BGRA
			//dst->VideoDecode() = thread->GetCompose()->SelfThread(thread);
			dst->Group() = thread->Group();
			if (cach) cach->ReleaseSws();
			cach = dst;
		}
		return cach->Copy(src);
	}

	bool CFrameBuffer::CreateTexture2D() {
		if (_Texture2D)return true;
		//FSlateRenderer* Renderer = FSlateApplication::Get().GetRenderer();
		//if (nullptr == Renderer || Renderer->HasLostDevice()) {
		//	return false;
		//}
		//auto RHIRef = new FSlateTexture2DRHIRef(decodeData.GetWidth(), decodeData.GetHeight(), Format, nullptr, TexCreate_Dynamic, true);
		// if (IsInRenderingThread()) {
		// 	RHIRef->InitResource();
		// }
		// else {
		// 	BeginInitResource(RHIRef);
		// }
		//BeginInitResource(RHIRef);
		//_Texture = RHIRef;
		_Texture2D = UTexture2D::CreateTransient(decodeData.GetWidth(), decodeData.GetHeight(), Format);
		_Texture2D->AddToRoot();
		return true;
	}
	void CFrameBuffer::UpdateTexture2D() {
		if (!_Texture2D)return;
		auto& data = decodeData.GetRawBytes();
		AsyncTask(ENamedThreads::GameThread, [&data = data,Texture2D = _Texture2D]() {
#if (UE_FF_VERSION) < 50000
			auto PlatformData = Texture2D->PlatformData;
#else
			auto PlatformData = Texture2D->GetPlatformData();
#endif
			void* TextrueData = PlatformData->Mips[0].BulkData.Lock(LOCK_READ_WRITE);
			FMemory::Memcpy(TextrueData, data.GetData(), data.Num());
			PlatformData->Mips[0].BulkData.Unlock();
			Texture2D->UpdateResource(); //更新资源
			});
	}

	//void CFrameBuffer::ReleaseTexture2D() {
	//	if (!_Texture2D)return ;
	//	AsyncTask(ENamedThreads::GameThread,[_Texture2D = _Texture2D]() {
	//		_Texture->Cleanup();
	//		});
	//	_Texture2D = nullptr;
	//}
	//std::shared_ptr<FThread>& CFrameBuffer::VideoDecode() {
	//	return Decode;
	//}
}
