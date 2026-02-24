// Game/Director/SceneCrossfadeController.h
#ifndef GAME_DIRECTOR_SCENECROSSFADECONTROLLER_H
#define GAME_DIRECTOR_SCENECROSSFADECONTROLLER_H

#include <string>

namespace Salt2D::Game::Director {

class SceneCrossfadeController {
public:
    void Reset() {
        active_ = false;
        elapsed_ = 0.0f;
        t01_ = 1.0f;
        lockPrev_ = false;
        lastNodeId_.clear();
        lastSpeaker_.clear();
        lastSerial_ = -1;
        hasLast_ = false;
    }

    void SetDuration(float sec) { duration_ = (sec > 0.0f ? sec : 0.01f); }

    void UpdateVN(const std::string& nodeId,
        const std::string& speaker, int lineSerial,
        bool cancel, float dtSec
    ) {
        if (nodeId != lastNodeId_) {
            Reset();
            lastNodeId_  = nodeId;
            lastSerial_  = lineSerial;
            lastSpeaker_ = speaker;
            return;
        }

        const bool isNewLine = hasLast_ && (lineSerial != lastSerial_);
        const bool isSameSpeaker = hasLast_ && (speaker == lastSpeaker_);

        if (isNewLine && isSameSpeaker) {
            active_ = true;
            elapsed_ = 0.0f;
            t01_ = 0.0f;
            lockPrev_ = true;
        }

        hasLast_ = true;
        lastSerial_ = lineSerial;
        lastSpeaker_ = speaker;

        if (!active_) { t01_ = 1.0f; lockPrev_ = false; return; }

        elapsed_ += dtSec;
        float u = elapsed_ / duration_;
        if (u >= 1.0f) { u = 1.0f, active_ = false; }

        t01_ = u;
        lockPrev_ = (u < 1.0f);

        if (cancel) {
            active_ = false;
            t01_ = 1.0f;
            lockPrev_ = false;
        }
    }

    float CrossfadeT() const { return t01_; }
    bool  LockPrev() const { return lockPrev_; }
    bool  Active() const { return active_; }

private:
    float duration_ = 0.5f;
    float elapsed_ = 0.0f;
    bool  active_ = false;

    float t01_ = 1.0f;
    bool  lockPrev_ = false;

    std::string lastNodeId_;
    std::string lastSpeaker_;
    int  lastSerial_ = -1;
    bool hasLast_ = false;
};

} // namespace Salt2D::Game::Director

#endif // GAME_DIRECTOR_SCENECROSSFADECONTROLLER_H
