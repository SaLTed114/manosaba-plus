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
        size_t totalCp  = 0; // in codepoints
        float revealCpF = 0.0f;
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

    struct DebateView {
        int statementIndex = 0;
        int statementCount = 0;

        std::string speaker;
        std::string fullText;
        std::string prefId;

        std::vector<std::string> spanIds;

        bool menuOpen = false;
        std::string openedSpanId;
        // {optionId, label}
        std::vector<std::pair<std::string, std::string>> options;

        float stmtTotalSec  = 0.0f;
        float stmtRemainSec = 0.0f;

        float timeScale = 1.0f;
    };

    std::optional<DebateView> debate;

    struct ChoiceView {
        // {optionId, label}
        std::vector<std::pair<std::string, std::string>> options;
    };

    std::optional<ChoiceView> choice;

    struct TimerView {
        bool active = false;
        float totalSec = 0.0f;
        float remainSec = 0.0f;
    };

    TimerView timer;
};

} // namespace Salt2D::Game::Story

#endif // GAME_STORY_STORYVIEW_H
