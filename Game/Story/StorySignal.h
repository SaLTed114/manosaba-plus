// Game/Story/StorySignal.h
#ifndef GAME_STORY_STORYSIGNAL_H
#define GAME_STORY_STORYSIGNAL_H

#include "StoryTypes.h"

namespace Salt2D::Game::Story {

struct StorySignal {
    enum class Kind : uint8_t {
        ChapterEnd,
        // placeholder for more signal kinds like "Intermission", "SpecialEvent", etc.
    };
    Kind kind = Kind::ChapterEnd;
    std::string key;
    NodeId nodeId;
};

} // namespace Salt2D::Game::Story

#endif // GAME_STORY_STORYSIGNAL_H
