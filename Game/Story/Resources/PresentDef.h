// Game/Story/Resources/PresentDef.h
#ifndef GAME_STORY_RESOURCES_PRESENTDEF_H
#define GAME_STORY_RESOURCES_PRESENTDEF_H

#include <string>
#include <vector>

namespace Salt2D::Game::Story {

struct PresentItem {
    std::string itemId;
    std::string label;
};

struct PresentDef {
    std::string prompt;
    std::vector<PresentItem> items;
};

} // namespace Salt2D::Game::Story

#endif // GAME_STORY_RESOURCES_PRESENTDEF_H
