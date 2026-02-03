// Game/Story/StoryView.h
#ifndef GAME_STORY_STORYVIEW_H
#define GAME_STORY_STORYVIEW_H

#include "StoryTypes.h"
#include <optional>
#include <string>

namespace Salt2D::Game::Story {

struct StoryView {
    NodeType nodeType = NodeType::Unknown;

    struct VnView {
        std::string speaker;
        std::string fullText;
        size_t revealed = 0; // in codepoints
        bool lineDone = false;
        bool finished = false;
    };

    std::optional<VnView> vn;

    struct PresentView {
        std::string prompt;
        // {itemId, label}
        std::vector<std::pair<std::string, std::string>> items;
    };

    std::optional<PresentView> present;

    // placeholder for 'Debate' and 'Present' view data
};

} // namespace Salt2D::Game::Story

#endif // GAME_STORY_STORYVIEW_H
