// Core/Time/FrameClock.cpp
#include "FrameClock.h"
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <algorithm>

namespace Salt2D::Core {

static int64_t QueryQPC() {
    LARGE_INTEGER qpc;
    QueryPerformanceCounter(&qpc);
    return qpc.QuadPart;
}

static int64_t QueryQPCFreq() {
    LARGE_INTEGER freq;
    QueryPerformanceFrequency(&freq);
    return freq.QuadPart;
}

void FrameClock::Reset() {
    initialized_ = true;
    paused_      = false;
    timeScale_   = 1.0;
    maxDtSec_    = 0.1;

    qpcFreq_     = QueryQPCFreq();
    lastQpc_     = QueryQPC();

    totalSec_    = 0.0;
    realSec_     = 0.0;
    frameIndex_  = 0;
}

FrameTime FrameClock::Tick() {
    if (!initialized_) Reset();

    int64_t currentQpc = QueryQPC();
    int64_t deltaQpc   = currentQpc - lastQpc_;
    lastQpc_ = currentQpc;

    double dtReal = (qpcFreq_ > 0) ? static_cast<double>(deltaQpc) / static_cast<double>(qpcFreq_) : 0.0;
    if (dtReal < 0.0) dtReal = 0.0; // QPC went backwards?
    if (maxDtSec_ > 0.0) dtReal = (std::min)(dtReal, maxDtSec_);

    realSec_ += dtReal;

    double dtScaled = paused_ ? 0.0 : dtReal * timeScale_;
    totalSec_ += dtScaled;

    FrameTime frameTime;
    frameTime.dtSec      = dtScaled;
    frameTime.totalSec   = totalSec_;
    frameTime.realSec    = realSec_;
    frameTime.frameIndex = frameIndex_++;
    return frameTime;
}
    
} // namespace SaltRT::Core
