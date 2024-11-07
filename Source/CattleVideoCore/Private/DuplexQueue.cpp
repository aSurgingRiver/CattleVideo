
#include "DuplexQueue.h"
#include "CattleVideoLog.h"
#include <thread>
#include <chrono>


namespace cattlevideo {
    CQueue::CQueue() {
        queueCount = 0;
        queueMax = 0;
    }
    CQueue::~CQueue() {
        Clear();
    }
    uint32 CQueue::Max() {
        return queueMax;
    }
    void CQueue::Max(uint32 max) {
        queueMax = max;
    }

    bool CQueue::Push(AVPacket* ffPkg) {
        AVPacket* p=nullptr;
        const uint32 queueMax_ = queueCount;
        if (!ffPkg) {// End packet
            p = ffPkg;
        }
        else if (queueMax_ < queueMax) {
            p = FFmpegCall(av_packet_clone)(ffPkg);
        }
        else {
            return false;
        }
        queue.Enqueue(p);
        queueCount++;
        return true;
    }
    bool CQueue::Pull(AVPacket*& ffPkg) {
        if (!queue.Dequeue(ffPkg)) {
            return false;
        }
        queueCount--;
        return true;
    }

    void CQueue::Clear() {
        AVPacket* pkg=nullptr;
        while (queue.Dequeue(pkg)) if (pkg)FFmpegCall(av_packet_free)(&pkg);
        queueCount = 0;
    }

}


