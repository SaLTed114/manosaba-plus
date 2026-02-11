// Game/Story/StoryTables.h
#ifndef GAME_STORY_STORYTABLES_H
#define GAME_STORY_STORYTABLES_H

#include "Game/Story/Resources/PerformanceDef.h"
#include "Game/Story/Resources/CastDef.h"
#include "Game/Story/Resources/StageDef.h"

namespace Salt2D::Game::Story {

struct StoryTables {
    PerformanceTable perf;
    CastTable cast;
    StageTable stage;
    // placeholder for more tables like Present Catalog, etc.
};

} // namespace Salt2D::Game::Story

#endif // GAME_STORY_STORYTABLES_H
