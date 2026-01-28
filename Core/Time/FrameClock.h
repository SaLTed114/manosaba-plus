// Core/Time/FrameClock.h
#ifndef CORE_TIME_FRAMECLOCK_H
#define CORE_TIME_FRAMECLOCK_H

#include <cstdint>

namespace Salt2D::Core {

struct FrameTime {
    double dtSec    = 0.0;
    double totalSec = 0.0;
    double realSec  = 0.0;
    uint64_t frameIndex = 0;
};

class FrameClock {
public:
    void Reset();

    FrameTime Tick();

    void SetPaused(bool paused) { paused_ = paused; }
    void SetTimeScale(double scale) { timeScale_ = scale; }
    void SetMaxDeltaTime(double maxDtSec) { maxDtSec_ = maxDtSec; }

    bool IsPaused() const { return paused_; }
    double GetTimeScale() const { return timeScale_; }
    double GetMaxDeltaTime() const { return maxDtSec_; }

private:
    bool initialized_ = false;
    bool paused_      = false;
    double timeScale_ = 1.0;
    double maxDtSec_  = 0.1;

    int64_t qpcFreq_ = 0;
    int64_t lastQpc_ = 0;

    double totalSec_ = 0.0;
    double realSec_  = 0.0;
    uint64_t frameIndex_ = 0;

};

} // namespace SaltRT::Core

#endif // CORE_TIME_FRAMECLOCK_H
