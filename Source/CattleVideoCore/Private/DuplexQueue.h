
#pragma once

#include "CoreMinimal.h"
#include "ffmpeg.hpp"
#include <atomic>
#include <mutex>
#include <queue>

namespace cattlevideo {
    //template<>
    class CQueue {
        //typedef TQueue<AVPacket*> Queue;
        uint16 queueMax ;
        std::atomic_uint queueCount ;
        TQueue<AVPacket*> queue;
        //std::function<void(AVPacket*)> freeFunc;
    public:
        CQueue();
        ~CQueue();
        uint32 Max();
        void Max(uint32 max);
        bool Push(AVPacket*);
        bool Pull(AVPacket*&);
        void Clear();
    };
}
