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
    float stmtBaseSec_ = 0.6f;
    float stmtCps_ = 12.0f; // characters (Chinese/Japanses) per second
    float stmtMinSec_ = 0.5f;
    float stmtMaxSec_ = 10.0f;
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

} // namespace Salt2D::Game::Story

#endif // GAME_STORY_STORYTIMER_H
