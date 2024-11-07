
#pragma once

#include "CoreMinimal.h"
#include "ffmpeg.hpp"
#include "DuplexQueue.h"
#include "HAL/Runnable.h"
#include "FrameBuffer.h"
#include <thread>
#include <mutex>
#include <atomic>
#include <functional>
class UCattleVideoControlImp;

namespace cattlevideo {
    static std::atomic_bool isPIE = true;
    class FThread;
    class AntCompose {
    public:
        virtual void UpdateTexture(std::shared_ptr<CFrameBuffer>, std::shared_ptr<CFrameBuffer>)=0;
        virtual void Duration(uint32_t) = 0;
        virtual void Position(uint32_t) = 0;
        virtual void ReOpen()=0;
        virtual void StartAudio(int InSampleRate, size_t NChannels)=0;
        virtual void AddAudio(const float* AudioData, int InSampleRate, size_t NChannels, size_t NFrames)=0;
        virtual std::shared_ptr<FThread> SelfThread(cattlevideo::FThread*)=0;
        virtual bool GetSize(int&,int&)=0;
        virtual void OnLoadStatus(FString, int, FString Desc)=0;
        virtual uint32 Speed() = 0;
        virtual void Speed(uint32 sp)=0;
    };

    enum ETHREAD_TYPE{
        ETHREAD_TYPE_READ=1,
        ETHREAD_TYPE_VIDEO = 2,
        ETHREAD_TYPE_AUDIO = 3,
        ETHREAD_TYPE_SUBTITLE = 4,
    };

    class FThread //: public FRunnable
    {
    public:
        FThread(ETHREAD_TYPE N, FGuid G);
        virtual ~FThread();

        //FRunnableThread* Thread();

        bool isRun();
        void Start();
        virtual void SetCompose(std::shared_ptr<AntCompose>);
        FGuid Group();
        virtual bool Init();
        virtual void Kill();
        std::shared_ptr<AntCompose> GetCompose();
        virtual void SetPause(bool pause);
        void AddCmd(std::function<void()>&);
        void AddCmd(std::function<void()>&&);
        void AddSyncCmd(std::function<void()>&,int ms = 10000);
        void AddSyncCmd(std::function<void()>&&,int ms = 10000);
    protected:
        bool    CmdMsg();
        void    RunThread();
        virtual uint32 Run();
    protected:
        //std::shared_ptr<FRunnableThread> myThread;
        ETHREAD_TYPE thdType;
        FString Name;
        std::atomic_bool bIsRunning;
        TQueue<std::function<void()>> Queue;
        //std::shared_ptr<AntCompose> Compose;
        std::shared_ptr<AntCompose> Compose;
        FGuid group;
        std::thread thd;
        bool is_pause;
        int64_t pause_time;
    };
    struct Clock {
        int64_t localClock;
        int64_t pts;
    };
    class FStream : public FThread {
    protected:
        const int waitSleep;
        const AVCodec* Codec;
        AVStream* Stream ;
        AVCodecContext* CodecContext ;
        int   streamid ;
        cattlevideo::CQueue pkgBuff;
        AVFrame* Frame;
        std::atomic_bool isEnd ;
        bool  isSync;
        bool  isHardware;
        bool  progress_synchro;
        TQueue<AVFrame*> finishQueue;
        AVFrame* finishFrame;
        std::atomic_uint finishCnt;
        std::shared_ptr<std::atomic<int64_t>> syncClock;
        int64_t start_pts;
        int64_t preClock;
        int64_t durationMicro;
        FStream* SyncStream;
        //uint32_t finishScale;
        std::shared_ptr<uint8_t> AsyncBatch;
        volatile uint8_t ReadBatch;
        volatile uint8_t FrameBatch;
    protected:
        virtual uint32 Run();
        virtual void BeginRun();
        void ReceiveFrame();
        void ExecuteFrame();
        virtual bool OnFrame(AVFrame* F) { return false; };
        virtual void EndDecode() {};
        void AddFinishFrame(AVFrame*);
        void ClearFinishFrame();
        void EmptyFrame();
        
    public:
        void SetSpeed(int sp);
        bool& Hardware();
        virtual void SetEnd(bool e);
        virtual void SetPause(bool pause);
        FStream(ETHREAD_TYPE N, FGuid G, unsigned int wait=50*100);
        ~FStream();
        void SetSyncClock(std::shared_ptr<std::atomic<int64_t>>);
        bool OpenCodecCtx(AVMediaType type, AVFormatContext* fmt_ctx, bool& Sync ,int initPkg,int& releat);
        bool IsOpen();
        bool PushPkg(AVPacket* Packet);
        int  StreamID();
        void SetSyncStream(FStream*);
        uint8_t BatchID();
        uint16_t FrameID();
        void Empty(uint8 Batch);
        void ProgressSynchro(bool synchro);
    };


    class FThreadRead : public FThread {
    private:
        int64_t EndReadTime;
        bool Repeat;
        AVPacket* Packet = nullptr;
        AVFormatContext* fmt_ctx = nullptr;
        //FStream* StreamA[AVMEDIA_TYPE_NB+1];
        std::shared_ptr<FStream> vedio;
        std::shared_ptr<FStream> audio;
        uint8_t ReadBatch;
        FString localRoot;
        //std::shared_ptr<FStream> subtitle;
    public:
        struct Option{
            Option() {
                PkgCacheNumber = 30;
                WaitTime = 180;
                Repeat = false;
                Hardware = false;
                //FFmpegDictionary;
            }
            int PkgCacheNumber;
            int WaitTime;
            bool Repeat;
            bool Hardware;
            TMap<FString, FString> FFmpegDictionary;
        };
    public:
        FThreadRead();
        ~FThreadRead();
        virtual uint32 Run();
        bool    Open(FString URL, Option opt);
        virtual void SetCompose(std::shared_ptr<AntCompose> C);
        virtual void SetPause(bool pause);
        std::shared_ptr<FThread> ParseThread(FThread* self);
        //virtual void Group(FGuid);
        virtual void Kill();
        int Interrupt();
        void SetSpeed(int ms);
        int Seek(int ms,int flag=0,bool wait=true);
        void ProgressSynchro(bool synchro);
    private:
        void OnLoadStatus(FString, int, FString Desc);
        bool OpenInternal();
        Option opt;
        FString URL;
    };

    class FVedioDecode : public FStream {
    private:
        std::shared_ptr<CFrameBuffer> Buffer;
        //int64_t referStartTime;
        //int64_t referEndTime;
        int64_t localClock;
        uint8_t textureFlag;
    protected:
        //virtual void BeginRun() ;
        virtual bool OnFrame(AVFrame* F);
        void UpdateTexture(std::shared_ptr<CFrameBuffer> NEW, std::shared_ptr<CFrameBuffer> OLD, bool newCache);
        virtual void EndDecode();
    public:
        FVedioDecode(FGuid G);
        ~FVedioDecode();
    };

    class FAudioDecode : public FStream {
    private:
        //std::vector<uint8> decodeData;
        //SwsContext* img_convert_ctx = nullptr;
    protected:
        virtual void BeginRun() ;
        virtual bool OnFrame(AVFrame* F);
    public:
        FAudioDecode(FGuid G,int waitmicro=5);
        ~FAudioDecode();
    private:
        struct AudioPar {
            AVSampleFormat format;//->AV_SAMPLE_FMT_FLT
            int sample_rate;
            AVChannelLayout ch_layout;
        };
        AudioPar want_param;
        AudioPar pre_param;
        struct SwrContext *swr_ctx;
        int nb_samples;
        //uint8_t* audio_buf1;
        //std::vector<uint8_t> data;
        uint8_t **dst_data = nullptr;
        int dst_linesize;
        unsigned int audio_buf1_size;
    };

    class FSubtitleDecode : public FStream {
    private:
        //std::vector<uint8> decodeData;
        //SwsContext* img_convert_ctx = nullptr;
    protected:
        //virtual bool OnFrame(AVFrame* F);
    public:
        FSubtitleDecode(FGuid G);
    };
}

