#pragma once
#include "ffmpeg.hpp"
#include <memory>

namespace cattlevideo {
	class FThread;
	class FVedioDecode;
	class CFrameBuffer {
	public:
		CFrameBuffer(AVPixelFormat src, int w, int h);
		~CFrameBuffer();
		bool InitSws(AVPixelFormat dst, int w, int h);
		void ReleaseSws();
		bool Compare(AVPixelFormat src, int w, int h);
		bool Copy(AVFrame* F);
		//FSlateUpdatableTexture* Texture();
		UTexture2D* Texture2D();
		bool CreateTexture2D();
		void UpdateTexture2D();
		//void ReleaseTexture2D();
		//void SetCache(std::shared_ptr<CFrameBuffer> R);
		//std::shared_ptr<CFrameBuffer>& CacheBuffer();
		void ClearDecodeData();
		void ReleaseCache();
		AVPixelFormat TransFormat(AVPixelFormat src);
		FGuid& Group();
		//std::shared_ptr<FThread>& VideoDecode();
	private:
		// FFmpeg info
		const AVPixelFormat srcFormat;
		std::vector<uint8> midData;
		FSlateTextureData decodeData;
		SwsContext* swsContext;
		uint8_t* pixels[8];
		int pitch[8];

		// UE texture info
		EPixelFormat  Format;
		//FSlateUpdatableTexture* _Texture;
		UTexture2D* _Texture2D;
		std::shared_ptr<CFrameBuffer> cache_buffer;
		uint8_t CacheDelay;
		FGuid group;
		//std::shared_ptr<FThread> Decode;
		// Thread info
		
		//int loop = 0;
	};
	bool Copy(AVFrame* src,std::shared_ptr<CFrameBuffer>&, FVedioDecode* thread);
}
