

#include "CattleVideoData.h"
#include "CattleVideoLog.h"
#include "Async/Async.h"
#include "HAL/RunnableThread.h"
#include "Misc/Paths.h"
#include "RenderingThread.h"
#include "Misc/ConfigCacheIni.h"
#include "CattleConfig.h"
#include "ffmpeg.hpp"
#if PLATFORM_WINDOWS
#include "Windows/WindowsHWrapper.h"
#endif
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
//#if FFMPEG_TEST
namespace cattlevideo {

    FThread::FThread(ETHREAD_TYPE t, FGuid G)
    {
        group = G;
        bIsRunning = false;
        is_pause = false;
        Compose = nullptr;
        thdType = t;
        FString N;
        switch (thdType) {
        case   ETHREAD_TYPE_READ:N = TEXT("Read"); break;
        case   ETHREAD_TYPE_VIDEO:N = TEXT("Video"); break;
        case   ETHREAD_TYPE_AUDIO:N = TEXT("Audio"); break;
        case   ETHREAD_TYPE_SUBTITLE:N = TEXT("Subtitle"); break;
        default:N = TEXT("Cattle");
        }
        Name = FString::Printf(TEXT("%s_%s"), *N, *group.ToString());
    }
    uint32 FThread::Run() {
        return 0;
    }

    void FThread::RunThread() {
        FPlatformProcess::SetThreadName(*Name);
        Init();
        Run();
    }

    void FThread::Start() {
        if (!Compose)return;
        std::shared_ptr<cattlevideo::FThread> self = Compose->SelfThread(this);
        std::thread t([self, Compose = Compose]() {
            self->RunThread();
            });
        t.swap(thd);
        UE_LOG(CattleVideoLog, Log, TEXT("FThread::Start Name:%s Group:%s"), *Name, *group.ToString());
    }

    void FThread::SetCompose(std::shared_ptr<AntCompose> C) {
        Compose = C;
    }
    std::shared_ptr<AntCompose> FThread::GetCompose() {
        return Compose;
    }
    void FThread::SetPause(bool pause) {
        is_pause = pause;
        std::function<void()> fun([pause, &is_pause = is_pause]()->void {
            is_pause = pause;
            });
        Queue.Enqueue(fun);
    }
    class SyncCmd {
    public:
        SyncCmd(std::function<void()>& Cmd) :fun(Cmd){
            Init();
        }
        SyncCmd(std::function<void()>&& Cmd) :fun(Cmd) {
            Init();
        }
        ~SyncCmd() {
            if(Event) FPlatformProcess::ReturnSynchEventToPool(Event);
            Event = nullptr;
        }
        void Excute() {
            fun();
            Event->Trigger();
        }
        void Wait(int ms) {
            Event->Wait(ms);
        }
    protected:
        void Init() {
            Event = FPlatformProcess::GetSynchEventFromPool();
        }
    private:
        std::function<void()> fun;
        FEvent* Event;
    };

    void FThread::AddCmd(std::function<void()>& Cmd) {
        Queue.Enqueue(Cmd);
    }
    void FThread::AddCmd(std::function<void()>&& Cmd) {
        Queue.Enqueue(Cmd);
    }
    void FThread::AddSyncCmd(std::function<void()>& Cmd, int ms) {
        auto cmd = std::make_shared<SyncCmd>(Cmd);
        AddCmd([cmd]() {cmd->Excute(); });
        cmd->Wait(ms);
    }
    void FThread::AddSyncCmd(std::function<void()>&& Cmd, int ms) {
        auto cmd = std::make_shared<SyncCmd>(Cmd);
        AddCmd([cmd]() {cmd->Excute(); });
        cmd->Wait(ms);
    }

    FGuid FThread::Group() {
        return group;
    }

    FThread::~FThread() {
        Kill();
        UE_LOG(CattleVideoLog, Log, TEXT("FThread::~FThread  Name:%s Group:%s"), *Name, *group.ToString());
    }

    void FThread::Kill() {
        if (thd.native_handle()) {
            thd.detach();
            /*if(isRun()) *///myThread->Kill();
        }
        bIsRunning = false;
    }

    FString ErrorString(int error) {
        char av_error[AV_ERROR_MAX_STRING_SIZE] = { 0 };
        FFmpegCall(av_strerror)(error, av_error, AV_ERROR_MAX_STRING_SIZE);
        return UTF8_TO_TCHAR(av_error);
    }

    bool FThread::Init() {
        bIsRunning = true;
        return true;
    }

    bool FThread::isRun() {
        return bIsRunning && isPIE;
    };

    bool IsSuccessed(int error, FString& Desc) {
        if (0 <= error)return true;
        Desc = ErrorString(error) + FString::Printf(TEXT("[%d]"), error);
        return false;
    }

    bool   FThread::CmdMsg() {
        std::function<void()> func;
        bool isCmd = false;
        while (isRun() && Queue.Dequeue(func)) {
            if (func)func();
            isCmd = true;
        }
        return isCmd;
    }

    FStream::FStream(ETHREAD_TYPE N, FGuid G, unsigned int wait) :FThread(N, G) , waitSleep(wait){
        Codec = nullptr;
        Stream = nullptr;
        CodecContext = nullptr;
        streamid = -1;
        Frame = nullptr;
        isEnd = false;
        isSync = false;
        finishFrame = nullptr;
        finishCnt = 0;
        isHardware = false;
        SyncStream = this;
        ReadBatch = -1;
        progress_synchro = false;
        //finishScale = 50;
    }
    bool& FStream::Hardware() {
        return isHardware;
    }

    FStream::~FStream() {
        SyncStream = nullptr;
        do {
            if (!finishFrame)continue;
            FFmpegCall(av_frame_unref)(finishFrame);
            FFmpegCall(av_frame_free)(&finishFrame);
        } while (finishQueue.Dequeue(finishFrame));
        if (CodecContext) {
            FFmpegCall(avcodec_free_context)(&CodecContext);
            CodecContext = nullptr;
        }
        if (Frame) {
            FFmpegCall(av_frame_unref(Frame));
            FFmpegCall(av_frame_free(&Frame));
            Frame = nullptr;
        }
    }
    void FStream::SetPause(bool pause) {
        if (!isSync) {
            FThread::SetPause(pause);
            return;
        }
        if (pause) {
            FThread::SetPause(pause);
            pause_time = FFmpegCall(av_gettime_relative)();
        }
        else {
            std::atomic<int64_t>& Clock = *syncClock;
            if (Clock != -1) {
                int64_t cur = FFmpegCall(av_gettime_relative)();
                Clock = Clock + cur - pause_time;
            }
            FThread::SetPause(pause);
        }
    }

    void FStream::ClearFinishFrame() {
        if (!finishFrame)return;
        FFmpegCall(av_frame_unref)(finishFrame);
        FFmpegCall(av_frame_free)(&finishFrame);
        finishFrame = nullptr;
    }
    void FStream::EmptyFrame() {
        ClearFinishFrame();
        AVFrame* emptyFrame=nullptr;
        while (finishQueue.Dequeue(emptyFrame)) {
            FFmpegCall(av_frame_unref)(emptyFrame);
            FFmpegCall(av_frame_free)(&emptyFrame);
            emptyFrame = nullptr;
        }
        finishCnt = 0;
    }
    void FStream::Empty(uint8 Batch) {
        ReadBatch = !isSync ? FrameBatch - 1 : FrameBatch - 2;
        AsyncBatch = std::make_shared<uint8>(Batch);
        EmptyFrame();
        pkgBuff.Clear();
        UE_LOG(CattleVideoLog, Log, TEXT("FThread::Empty Name:%s Group:%s ReadBatch:%d"), *Name, *group.ToString(), ReadBatch);
    }
    void FStream::ProgressSynchro(bool synchro) {
        progress_synchro = synchro;
    }
    void FStream::SetSpeed(int sp) {
        AddCmd([this, sp]() {
            ReadBatch = !isSync ? FrameBatch - 1 : FrameBatch - 2;
            if (!isSync) return;
            Compose->Speed(sp);
            });
        
    }

    void FStream::ExecuteFrame() {
        if (!finishFrame) {
            if (!finishQueue.Dequeue(finishFrame))return;
            finishCnt--;
        }
        FrameBatch = finishFrame->pkt_pos & 0xFF;
        if (AsyncBatch && *AsyncBatch != FrameBatch) {// force sync
            ClearFinishFrame();
            return;
        }
        if (ReadBatch != FrameBatch) {// 批次号改变
            start_pts = finishFrame->pts;
            //finishScale = 1000;
            uint16_t frameid = SyncStream->FrameID();
            if (frameid != FrameBatch) // 和同步流的ID 相等说明待处理的Frame是同一个批次的.
                return ;
            //finishScale = 50;
            AsyncBatch = nullptr;
            ReadBatch = FrameBatch;
            *syncClock = -1;//
            UE_LOG(CattleVideoLog, Log, TEXT("FStream::ExecuteFrame Name:%s Group:%s ReadBatch:%d"), *Name, *group.ToString(), ReadBatch);
        }
        if (SyncStream->BatchID() != FrameBatch) {// 等待对方更新ID
            return ;
        }
        std::atomic<int64_t>& StartClock = *syncClock;
        int64_t curTime = FFmpegCall(av_gettime_relative)();
        int64_t start_time = StartClock;
        if (start_time == -1) {
            if (!isSync) return ;
            preClock = start_time = StartClock = curTime;
        }
        int iSp = 100;
        if (Compose) {
            iSp = FMath::Clamp(int32(Compose->Speed()), 50, 300); 
        }
        float sp = durationMicro * 100.0 / iSp;
        int64_t pts = sp * (finishFrame->pts - start_pts);
        if (curTime < (start_time + pts))return ;
        if (isSync) {
            if (preClock + 10 * sp < curTime)start_time = StartClock = curTime - pts;
            if (progress_synchro) {
                int64_t playPos = finishFrame->pts * durationMicro;
                AsyncTask(ENamedThreads::Type::GameThread, [Compose = Compose, playPos]() {
                    Compose->Position(playPos);
                    });
            }
        }
        // fast play drop frame
        if ((100 < iSp) && (curTime < preClock + durationMicro)) {
            ClearFinishFrame();
            return;
        }
        else if (thdType == ETHREAD_TYPE_AUDIO && iSp != 100) {
            ClearFinishFrame();
            return;
        }
        preClock = curTime;
        if (!OnFrame(finishFrame)) {
            return;
        }
        ClearFinishFrame();
    }

    void FStream::ReceiveFrame() {
        int result = 0;
        FString Desc;
        for (; isRun();) {
            result = FFmpegCall(avcodec_receive_frame)(CodecContext, Frame);
            if (IsSuccessed(result, Desc)) {
                if (Frame->pts == AV_NOPTS_VALUE)Frame->pts = Frame->pkt_dts;
                AddFinishFrame(Frame);
                FFmpegCall(av_frame_unref)(Frame);
                continue;
            }
            FFmpegCall(av_frame_unref)(Frame);
            if (result == AVERROR(EAGAIN) || result == AVERROR_EOF) {
                break;
            }
            else {
                UE_LOG(CattleVideoLog, Error, TEXT("decoding frame Error[%s]"), *Desc);
                break;
            }
        }
    }

    void FStream::BeginRun() {
        start_pts = 0;
        durationMicro = av_q2d(Stream->time_base) * 1000000;
        if (!isSync)return;
        //if (isSync&&Compose)
        //    Compose->Duration(Stream->duration * durationMicro);
        int64_t duration = Stream->duration * durationMicro;
        AsyncTask(ENamedThreads::Type::GameThread, [Compose= Compose, duration]() {
            Compose->Duration(duration);
            });
    }

    uint32 FStream::Run() {
        BeginRun();
        Frame = FFmpegCall(av_frame_alloc)();
        FString Desc;
        int result = 0;
        AVPacket* pkg = nullptr;
        for (; isRun();) {
            if (CmdMsg())continue;// 
            if (is_pause) {
                std::this_thread::sleep_for(std::chrono::milliseconds(1000));
                continue;
            }
            ExecuteFrame();
            if (5 * pkgBuff.Max() <= finishCnt) {
                std::this_thread::sleep_for(std::chrono::microseconds(waitSleep));
                continue;
            }
            ReceiveFrame();
            if (result == AVERROR(EAGAIN)) {
                result = FFmpegCall(avcodec_send_packet)(CodecContext, pkg);
               if (!IsSuccessed(result, Desc)) {
                   if (result == AVERROR_EOF && finishCnt == 0) break;
                   continue;
               }
            }
            if (nullptr!=pkg) {
                FFmpegCall(av_packet_unref)(pkg);
                FFmpegCall(av_packet_free)(&pkg);
                pkg = nullptr;
            }
            if (!pkgBuff.Pull(pkg)) {
                if (isEnd && finishCnt == 0)break;
                std::this_thread::sleep_for(std::chrono::microseconds(waitSleep));
                continue;
            }
            result = AVERROR(EAGAIN);
        }
        UE_LOG(CattleVideoLog, Log, TEXT("Decode Stream End Name:%s Group:%s Run:%d"), *Name, *group.ToString(), bIsRunning?1:0);
        bIsRunning = false;
        if (pkg) {
            FFmpegCall(av_packet_unref)(pkg);
            FFmpegCall(av_packet_free)(&pkg);
        }
        if (Frame)FFmpegCall(av_frame_free)(&Frame);
        Frame = nullptr;
        EndDecode();
        return 0;
    }

    void FStream::AddFinishFrame(AVFrame* Src) {
        AVFrame* F = nullptr;
        if (isHardware && CodecContext->gop_size == Src->format) {
            F = FFmpegCall(av_frame_alloc)();
            FString Desc;
            if (!IsSuccessed(FFmpegCall(av_hwframe_transfer_data)(F, Src, 0),Desc)) {
                UE_LOG(CattleVideoLog, Error, TEXT("Error transferring the data to system memory"));
                FFmpegCall(av_frame_unref)(F);
                FFmpegCall(av_frame_free)(&F);
                return;
            }
            F->pkt_pos = Src->pkt_pos;
            F->pts = Src->pts;
            F->pkt_dts = Src->pkt_dts;
        }
        else {
            F = FFmpegCall(av_frame_clone)(Src);
        }
        //UE_LOG(CattleVideoLog, Log, TEXT("Frame type[%d] dts[%d] pts[%d]")
        //    , Stream->codecpar->codec_type
        //    , F->pkt_dts, F->pts);
        finishQueue.Enqueue(F);
        finishCnt++;
    }
    void FStream::SetSyncClock(std::shared_ptr<std::atomic<int64_t>> Clock) {
        syncClock = Clock;
    }

    void FStream::SetEnd(bool e) {
        isEnd = e;
        if (isRun())PushPkg(nullptr);
    }

    static enum AVPixelFormat get_hw_format(AVCodecContext* ctx,
        const enum AVPixelFormat* pix_fmts)
    {
        const enum AVPixelFormat* p;
        for (p = pix_fmts; *p != -1; p++) {
            if (*p == ctx->gop_size)
                return *p;
        }
        return AV_PIX_FMT_NONE;
    }

    bool FStream::OpenCodecCtx(AVMediaType type, AVFormatContext* fmt_ctx, bool& Sync, int initPkg, int& releat) {
        FString Desc;
        streamid = FFmpegCall(av_find_best_stream)(fmt_ctx, type, -1, releat, NULL, 0);
        if (!IsSuccessed(streamid, Desc)) {
            UE_LOG(CattleVideoLog, Warning, TEXT("Can't find [%d] stream [%s]"), type, *Desc);
            return false;
        }
        releat = streamid;
        Stream = fmt_ctx->streams[streamid];
        Stream->discard = AVDISCARD_ALL; // Add 
        AVCodecParameters* codecpar = Stream->codecpar;
        Codec = FFmpegCall(avcodec_find_decoder)(codecpar->codec_id);
        if (!Codec) {
            UE_LOG(CattleVideoLog, Error, TEXT("Can't find [%d] decoder"), type);
            return false;
        }
        CodecContext = FFmpegCall(avcodec_alloc_context3)(nullptr);
        if (!CodecContext) {
            UE_LOG(CattleVideoLog, Error, TEXT("Can't allocate [%d] decoder context"), type);
            return false;
        }
        if (!IsSuccessed(FFmpegCall(avcodec_parameters_to_context)(CodecContext, codecpar), Desc)) {
            UE_LOG(CattleVideoLog, Error, TEXT("Can't copy [%d] decoder context [%s]"), type, *Desc);
            return false;
        }

        //CodecContext->gop_size = -1;
        AVPixelFormat devFormat = AV_PIX_FMT_NONE;
        enum AVHWDeviceType devType = AV_HWDEVICE_TYPE_NONE;
        for (int i = 0; isHardware; i++) {
            if (4320 < codecpar->width || 2160 < codecpar->height)break;
            const AVCodecHWConfig* config = FFmpegCall(avcodec_get_hw_config)(Codec, i);
            if (!config) break;
            if ((config->methods & AV_CODEC_HW_CONFIG_METHOD_HW_DEVICE_CTX) == 0)continue;
            if (config->device_type == AV_HWDEVICE_TYPE_CUDA
                    || config->device_type == AV_HWDEVICE_TYPE_DXVA2
                    || config->device_type == AV_HWDEVICE_TYPE_D3D11VA
                    || config->device_type == AV_HWDEVICE_TYPE_QSV
                    || config->device_type == AV_HWDEVICE_TYPE_OPENCL
                    || config->device_type == AV_HWDEVICE_TYPE_VULKAN
                ) {
                devType = config->device_type;
                devFormat = config->pix_fmt;
                break;
            }
        }
        if (devFormat == AV_PIX_FMT_NONE) {
            isHardware = false;
        }
        else if (devType != AV_HWDEVICE_TYPE_NONE
            && IsSuccessed(FFmpegCall(av_hwdevice_ctx_create)(&CodecContext->hw_device_ctx, devType, NULL, NULL, 0), Desc)) {
            CodecContext->gop_size = devFormat;
            CodecContext->get_format = get_hw_format;
        }
        else isHardware = false;

        CodecContext->pkt_timebase = Stream->time_base;
        CodecContext->codec_id = Codec->id;
        CodecContext->lowres = Codec->max_lowres;
        CodecContext->framerate = Stream->avg_frame_rate;
        // 
        //auto guess_frame_rate = av_guess_frame_rate(fmt_ctx,Stream,nullptr);
        AVDictionary* options = nullptr;
        FFmpegCall(av_dict_set)(&options, "threads", "auto", 0);
        if (!IsSuccessed(FFmpegCall(avcodec_open2)(CodecContext, Codec, nullptr), Desc)) {
            UE_LOG(CattleVideoLog, Error, TEXT("Can't open [%d] decoder [%s]"), type, *Desc);
            return false;
        }
        if(options) FFmpegCall(av_dict_free)(&options);
        Stream->discard = AVDISCARD_DEFAULT;
        isSync = Sync;
        Sync = false;
        pkgBuff.Max(initPkg);

        return true;
    }

    bool FStream::IsOpen() {
        return Codec != nullptr;
    }
    bool FStream::PushPkg(AVPacket* Packet) {
        while (isRun()) {
            if (pkgBuff.Push(Packet))return true;
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        };
        return false;
    }
    int  FStream::StreamID() {
        return streamid;
    }
    void FStream::SetSyncStream(FStream* _Stream) {
        SyncStream = _Stream;
    }
    uint8_t FStream::BatchID() {
        return ReadBatch;
    }
    uint16_t FStream::FrameID() {
        return FrameBatch;
    }

    int interrupt_cb(void* R) {
        FThreadRead* Read = (FThreadRead*)R;
        return Read->Interrupt();
    }

    bool FThreadRead::Open(FString U, Option o) {
        if (U.StartsWith(TEXT("local://"))) {
            FString local = FPaths::Combine( FPaths::ProjectContentDir(),*localRoot);
            U = U.Replace(TEXT("local:/"), *local);
        }
        URL = U;
        opt = o;
        Start();
        return true;
    }

    bool FThreadRead::OpenInternal() {
        std::string url = TCHAR_TO_UTF8(*URL);
        bool needFastPlay=true;
        auto smallletter = URL.ToLower();
        // if (smallletter.StartsWith(TEXT("http")) || smallletter.StartsWith(TEXT("https"))) {
        //     needFastPlay = false;
        // }
        bool pullStream = false;
        if (smallletter.StartsWith(TEXT("rtsp")) || smallletter.StartsWith(TEXT("rtmp"))) {
            pullStream = true;
        }
        FString Desc;
        Repeat = opt.Repeat;
        AVDictionary* options = nullptr;
        bool hasLinkMod = false;
        if (opt.FFmpegDictionary.Num()) {
            std::string key;
            std::string value;
            for (auto it : opt.FFmpegDictionary) {
                if (it.Key.Len() == 0 || it.Value.Len() == 0)continue;
                key = TCHAR_TO_UTF8(*it.Key);
                value = TCHAR_TO_UTF8(*it.Value);
                FFmpegCall(av_dict_set)(&options, key.c_str(), value.c_str(), 0);
                if (TEXT("rtsp_transport") == it.Key) hasLinkMod = true;
            }
        }
        if (pullStream && hasLinkMod==false) {
            FFmpegCall(av_dict_set)(&options, "rtsp_transport", "tcp", 0);
        }
        bool ptms = false;
        if (!FFmpegCall(av_dict_get)(options, "scan_all_pmts", NULL, AV_DICT_MATCH_CASE)) {
            FFmpegCall(av_dict_set)(&options, "scan_all_pmts", "1", 0);
            ptms = true;
        }
        AVInputFormat* iformat = nullptr;
        fmt_ctx = FFmpegCall(avformat_alloc_context)();
        fmt_ctx->interrupt_callback.opaque = this; //C++
        fmt_ctx->interrupt_callback.callback = interrupt_cb;//设置回调函数，否则有可能ffmpeg一直被挂住。 
        EndReadTime = FFmpegCall(av_gettime_relative)() + opt.WaitTime;// 
        fmt_ctx->flags = fmt_ctx->flags & AVFMT_FLAG_NOBUFFER;
        if (!IsSuccessed(FFmpegCall(avformat_open_input)(&fmt_ctx, url.c_str(), nullptr, &options), Desc)) {
            if (pullStream == false || hasLinkMod == true) {// make sure ，not pull stream
                OnLoadStatus(URL, -2, FString::Printf(TEXT("Can't open file/url [%s][%s]"), *Desc, *URL));
                return false;
            }
            UE_LOG(CattleVideoLog, Warning, TEXT("url open failed by tcp, now use udp to link ..."));
            EndReadTime = FFmpegCall(av_gettime_relative)() + opt.WaitTime;//
            FFmpegCall(av_dict_set)(&options, "rtsp_transport", "udp", 0);
            if (!IsSuccessed(FFmpegCall(avformat_open_input)(&fmt_ctx, url.c_str(), nullptr, &options), Desc)) {
                OnLoadStatus(URL, -2, FString::Printf(TEXT("Can't open file/url [%s][%s]"), *Desc, *URL));
                return false;
            }
            UE_LOG(CattleVideoLog, Warning, TEXT("url open sucessed! by udp"));
        }
        if (ptms) FFmpegCall(av_dict_set)(&options, "scan_all_pmts", NULL, AV_DICT_MATCH_CASE);
        if (options) { FFmpegCall(av_dict_free)(&options); options = nullptr; }

        FFmpegCall(av_format_inject_global_side_data)(fmt_ctx); // ?
        //AVDictionary** opts = nullptr;
        //opts = (AVDictionary**)av_calloc(fmt_ctx->nb_streams, sizeof(*opts));
        if ((false==CCattleConfig::get()->FastPlay(needFastPlay)|| fmt_ctx->nb_streams == 0) && !IsSuccessed(FFmpegCall(avformat_find_stream_info)(fmt_ctx, nullptr), Desc)) {
            OnLoadStatus(URL, -2, FString::Printf(TEXT("Can't get stream info [%s][%s]"), *Desc, *URL));
            return false;
        }
        //for (unsigned int i = 0; i < fmt_ctx->nb_streams; i++)
        //    av_dict_free(&opts[i]);
        //av_freep(&opts);
        //if (fmt_ctx->pb) fmt_ctx->pb->eof_reached = 0;
        //FFmpegCall(av_dump_format)(fmt_ctx, 0, url.c_str(), 0);

        if (!fmt_ctx->nb_streams) {
            OnLoadStatus(URL, -3, FString::Printf(TEXT("Can't get any stream info nb_streams=0[%s]"), *URL));
            return false;
        }
        bool Sync = true;
        int StreamID = -1;
        std::shared_ptr<FStream> StreamPtr = nullptr;
        vedio->Hardware() = opt.Hardware;
        if (vedio->OpenCodecCtx(AVMEDIA_TYPE_VIDEO, fmt_ctx, Sync, opt.PkgCacheNumber, StreamID)) {
            vedio->Start(); audio->SetSyncStream(vedio.get()); if (!StreamPtr)StreamPtr = vedio;
        }
        if (audio->OpenCodecCtx(AVMEDIA_TYPE_AUDIO, fmt_ctx, Sync, opt.PkgCacheNumber * 200, StreamID)) {
            audio->Start(); vedio->SetSyncStream(audio.get()); if (!StreamPtr)StreamPtr = audio;
        }
        if (Sync) {
            OnLoadStatus(URL, -4, FString::Printf(TEXT("Can't get any stream info[%s]"), *URL));
            return false;
        }
        Packet = FFmpegCall(av_packet_alloc)();
        if (!Packet) {
            OnLoadStatus(URL, -5,FString::Printf(TEXT("Cannot allocate packet[%s]"),*URL));
            return false;
        }
        StreamPtr->AddSyncCmd([]() {},10000);
        OnLoadStatus(URL, 0,TEXT("Sucessed!"));
        return true;
    }

    void FThreadRead::OnLoadStatus(FString _URL, int state, FString Desc) {
        if(state)UE_LOG(CattleVideoLog, Error, TEXT("%s"), *Desc);
        AsyncTask(ENamedThreads::GameThread, [_URL, state, Desc, Compose= Compose]() {
            if(Compose) Compose->OnLoadStatus(_URL, state, Desc);
            });
    }

    int FThreadRead::Interrupt() {
        if (-1== EndReadTime||FFmpegCall(av_gettime_relative)() <= EndReadTime) {
            return 0;
        }
        else {
            UE_LOG(CattleVideoLog, Error, TEXT("FThreadRead::Interrupt timeout"));
            return 1;
        }
    }
    void FThreadRead::SetSpeed(int sp) {
        if (vedio)vedio->SetSpeed(sp);
        if (audio)audio->SetSpeed(sp);
 /*       AddCmd([this, sp]() {
            ReadBatch++;
            Compose->Speed(sp);
        });*/
    }

    int FThreadRead::Seek(int ms, int flag,bool wait) {
        ReadBatch++;
        if (wait) {
            if (vedio)vedio->AddSyncCmd([vedio = vedio.get(), ReadBatch = ReadBatch]() {vedio->Empty(ReadBatch); },100000);
            if (audio)audio->AddSyncCmd([audio = audio.get(), ReadBatch = ReadBatch]() {audio->Empty(ReadBatch); }, 100000);
        }

        auto re = FFmpegCall(avformat_seek_file)(fmt_ctx, -1, INT64_MIN, ms, INT64_MAX, flag);
        UE_LOG(CattleVideoLog, Log, TEXT("FThreadRead::Seek Name:%s Group:%s postion:%d ReadBatch:%d"), *Name, *group.ToString(), ms, ReadBatch);
        return re;
    }
    void FThreadRead::ProgressSynchro(bool synchro) {
        if (vedio)vedio->AddCmd([vedio = vedio.get(), synchro]() {vedio->ProgressSynchro(synchro); });
        if (audio)audio->AddCmd([audio = audio.get(), synchro]() {audio->ProgressSynchro(synchro); });
    }

    void FThreadRead::Kill() {
        EndReadTime = 1;
        if (vedio)vedio->Kill();
        if (audio)audio->Kill();
        FThread::Kill();
        UE_LOG(CattleVideoLog, Log, TEXT("FThreadRead::OnKill"));
    }
    FThreadRead::FThreadRead()
        :FThread(ETHREAD_TYPE_READ, FGuid::NewGuid())
    {
        localRoot = TEXT("cattlevideo");
        GConfig->GetString(TEXT("CattleVideo"), TEXT("local_root"), localRoot, GGameIni);
        ReadBatch = 0; 
        Repeat = false;
        vedio = std::make_shared<FVedioDecode>(group);
        audio = std::make_shared<FAudioDecode>(group);
        auto clock = std::make_shared<std::atomic<int64_t>>(-1);
        vedio->SetSyncClock(clock);
        audio->SetSyncClock(clock);
    }
    FThreadRead::~FThreadRead() {
        vedio = nullptr;
        audio = nullptr;
        if (Packet) {
            FFmpegCall(av_packet_unref)(Packet);
            FFmpegCall(av_packet_free)(&Packet);
        }
        if (fmt_ctx)FFmpegCall(avformat_close_input)(&fmt_ctx);
        Packet = nullptr;
        fmt_ctx = nullptr;
    }
    void FThreadRead::SetCompose(std::shared_ptr<AntCompose> C) {
        FThread::SetCompose(C);
        vedio->SetCompose(C);
        audio->SetCompose(C);
    }

    void FThreadRead::SetPause(bool pause) {
        FThread::SetPause(pause);
        Queue.Enqueue([this]() {
            EndReadTime = -1;// FFmpegCall(av_gettime_relative)() + 100000 * 1000000;
            });
        vedio->SetPause(pause);
        audio->SetPause(pause);
    }

    std::shared_ptr<FThread> FThreadRead::ParseThread(FThread* self) {
        if (self == vedio.get())return vedio;
        if (self == audio.get())return audio;
        return nullptr;
    }

    uint32 FThreadRead::Run() {
        if (!Compose)return 0;
        std::shared_ptr<cattlevideo::FThread> self = Compose->SelfThread(this);
        if (!OpenInternal())return 0;
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        FString Desc;
        int result = 0;
        FStream* stream;
        for (; isRun();) {
            if (CmdMsg())continue;// 
            if (is_pause) {
                std::this_thread::sleep_for(std::chrono::milliseconds(1000));
                continue;
            }
            EndReadTime = FFmpegCall(av_gettime_relative)() + opt.WaitTime;
            result = FFmpegCall(av_read_frame)(fmt_ctx, Packet);
            if (!IsSuccessed(result, Desc)) {
                FFmpegCall(av_packet_unref)(Packet);
                if (AVERROR_EOF==result && Repeat) {
                    result = Seek(0,0,false);
                    continue;
                }
                UE_LOG(CattleVideoLog, Log, TEXT("Read stream info Stop Name:%s Group:%s Desc:%s Error[%d]"), *Name, *group.ToString(), *Desc, result);
                break;
            }
            stream = nullptr;
            if (Packet->stream_index == vedio->StreamID())
                stream = vedio.get();
            else if (Packet->stream_index == audio->StreamID())
                stream = audio.get();
            Packet->pos = Packet->pos <<8 | ReadBatch;
            if (stream)stream->PushPkg(Packet);
            FFmpegCall(av_packet_unref)(Packet);
        }
        if (vedio) vedio->SetEnd(true);
        if (audio) audio->SetEnd(true);
        if (((-1 != EndReadTime && EndReadTime <= FFmpegCall(av_gettime_relative)()) || result == -138) && isRun() && Compose && Repeat) {
            Compose->ReOpen();
        }
        bIsRunning = false;
        UE_LOG(CattleVideoLog, Log, TEXT("Read stream thread End Name:%s Group:%s"), *Name, *group.ToString());
        return 0;
    }

    FVedioDecode::FVedioDecode(FGuid G)
        : FStream(ETHREAD_TYPE_VIDEO,G)
    {
        Buffer = nullptr;
        ReadBatch = -2;
        localClock = -1;
    }
    FVedioDecode::~FVedioDecode() {
        Buffer = nullptr;
    }
    bool FVedioDecode::OnFrame(AVFrame* F) {
        std::shared_ptr<CFrameBuffer> cach = Buffer;
        if (!cattlevideo::Copy(F, Buffer,this))return false;
        if (cach == Buffer)cach = nullptr;
        bool newCache = false;
        if (nullptr == Buffer->Texture2D()) {
            Buffer->CreateTexture2D();
            newCache = true;
        }
        UpdateTexture(Buffer, cach, newCache);
        return true;
    }

    void FVedioDecode::UpdateTexture(std::shared_ptr<CFrameBuffer> NEW, std::shared_ptr<CFrameBuffer> OLD,bool newCache) {
        if (!Compose)return;
        ENQUEUE_RENDER_COMMAND(FVedioDecode_UpdateTexture)(
            [Compose = Compose, NEW, OLD, newCache](FRHICommandListImmediate& RHICmdList) {
                NEW->UpdateTexture2D();
                if (newCache == false)return;
                AsyncTask(ENamedThreads::GameThread, [Compose, NEW, OLD]() {
                    Compose->UpdateTexture(NEW, OLD);
                    });
            });
    }
    void FVedioDecode::EndDecode() {
        if (!Buffer)return;
        Buffer->ClearDecodeData();
        UpdateTexture(Buffer,nullptr,false);
    }

    FAudioDecode::FAudioDecode(FGuid G,int waitmicro)
        : FStream(ETHREAD_TYPE_AUDIO, G, waitmicro)
    {
        swr_ctx = nullptr;
        audio_buf1_size = 0;
    }
    void FAudioDecode::BeginRun() {
        FStream::BeginRun();
        int sample_rate = CodecContext->sample_rate;
        int nb_channels = get_channel_layout(CodecContext).nb_channels;
        AsyncTask(ENamedThreads::GameThread, [Compose=Compose, sample_rate, nb_channels]() {
            Compose->StartAudio(sample_rate, nb_channels);
            });
        want_param.format = AV_SAMPLE_FMT_FLT;
        want_param.sample_rate = CodecContext->sample_rate;
        auto from_ch_layout = get_channel_layout(CodecContext);
        FFmpegCall(av_channel_layout_copy)(&want_param.ch_layout, &from_ch_layout);
        //want_param.ch_layout.nb_channels = 1;
        //want_param.ch_layout.order = AV_CHANNEL_ORDER_UNSPEC;
        pre_param = want_param;
    }
    bool FAudioDecode::OnFrame(AVFrame* F) {
        FString Desc;
        auto frame_ch_layout = get_channel_layout(F);
        if (F->format != pre_param.format 
            || pre_param.sample_rate != F->sample_rate
            || FFmpegCall(av_channel_layout_compare)(&frame_ch_layout, &pre_param.ch_layout)
            ){
            if(swr_ctx)FFmpegCall(swr_free)(&swr_ctx);
            swr_ctx = nullptr;
            if (!IsSuccessed(FFmpegCall(swr_alloc_set_opts2)(&swr_ctx,
                &want_param.ch_layout, want_param.format, want_param.sample_rate,
                &frame_ch_layout, (AVSampleFormat)F->format, F->sample_rate,
                0, NULL),Desc)) {
                return true;
            }
            if (!IsSuccessed(FFmpegCall(swr_init)(swr_ctx), Desc)) {
                return true;
            }
            if (FFmpegCall(av_samples_alloc_array_and_samples)(&dst_data, &dst_linesize, want_param.ch_layout.nb_channels, F->nb_samples, want_param.format, 0)<0) {
                //av_log(NULL, AV_LOG_ERROR, "Could not allocate destination samples\n");
                UE_LOG(CattleVideoLog, Error, TEXT("Could not allocate destination samples\n"));
                return true;
            }
            if (FFmpegCall(av_channel_layout_copy)(&pre_param.ch_layout, &frame_ch_layout) < 0)return true;
            
            pre_param.format =(AVSampleFormat) F->format;
            pre_param.sample_rate = F->sample_rate;
        }
        uint8_t* outBuf;
        if (swr_ctx) {
            if (nb_samples != F->nb_samples) {
                FFmpegCall(av_freep)(&dst_data[0]);
                if (FFmpegCall(av_samples_alloc)(dst_data, &dst_linesize, want_param.ch_layout.nb_channels, F->nb_samples, want_param.format, 1)<0) {
                    return true;
                }
                nb_samples = F->nb_samples;
            }
            int len2 = FFmpegCall(swr_convert)(swr_ctx, dst_data, nb_samples,(const uint8_t **) F->data, F->nb_samples);
            if (!IsSuccessed(len2, Desc)) {
                UE_LOG(CattleVideoLog, Error, TEXT("swr_convert() failed\n"));
                return true;
            }
            outBuf = dst_data[0];
        }
        else {
            outBuf = F->data[0];
        }
        if (!Compose)return true;
        Compose->AddAudio((float*)outBuf,want_param.sample_rate,want_param.ch_layout.nb_channels,F->nb_samples);
        return true;
    }

    FAudioDecode::~FAudioDecode() {
        if (dst_data) {
            FFmpegCall(av_freep)(&dst_data[0]);
            FFmpegCall(av_freep)(&dst_data);
            dst_data = nullptr;
        }
        if (swr_ctx) {
            FFmpegCall(swr_free)(&swr_ctx);
            swr_ctx = nullptr;
        }
    }
    FSubtitleDecode::FSubtitleDecode(FGuid G)
        : FStream(ETHREAD_TYPE_SUBTITLE, G)
    {
    }
}

//#endif