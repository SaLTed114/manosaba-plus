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

} // namespace Salt2D::Game::Story

#endif // GAME_STORY_STORYTIMER_H
