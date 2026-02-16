// Game/Story/StoryTimer.h
#ifndef GAME_STORY_STORYTIMER_H
#define GAME_STORY_STORYTIMER_H

#include <optional>
#include "StoryTypes.h"

namespace Salt2D::Game::Story {

struct NodeTimer {
    bool active = false;
    NodeId lastActiveNodeId;

    float totalSec  = 0.0f;
    float remainSec = 0.0f;
    std::optional<NodeId> beNode; // for BE nodes, the node to jump to when time is up

    void Reset() {
        active = false;
        totalSec = 0.0f;
        remainSec = 0.0f;
        beNode.reset();
    }
};

struct StatementTimeConfig {
    float stmtBaseSec = 0.6f;
    float stmtCps = 12.0f; // characters (Chinese/Japanses) per second
    float stmtMinSec = 0.5f;
    float stmtMaxSec = 10.0f;
};

struct StatementTimer {
    bool active = false;
    int statementIndex = -1;

    float totalSec  = 0.0f;
    float remainSec = 0.0f;

    StatementTimeConfig cfg;

    void Reset() {
        active = false;
        statementIndex = -1;
        totalSec = 0.0f;
        remainSec = 0.0f;
    }
};

struct VnAutoTimerConfig {
    float baseSec = 0.3f;
    float cps = 18.0f;
    float minSec = 0.8f;
    float maxSec = 1.5f;
};

struct VnAutoTimer {
    bool active = false;
    int lineSerial = 0;
    float totalSec = 0.0f;
    float remainSec = 0.0f;

    VnAutoTimerConfig cfg;

    void Reset() {
        active = false;
        totalSec = 0.0f;
        remainSec = 0.0f;
        lineSerial = 0;
    }
};

} // namespace Salt2D::Game::Story

#endif // GAME_STORY_STORYTIMER_H
