// Game/Story/Resources/DebateDef.h
#ifndef GAME_STORY_RESOURCES_DEBATEDEF_H
#define GAME_STORY_RESOURCES_DEBATEDEF_H

#include <string>
#include <vector>

namespace Salt2D::Game::Story {

struct DebateStatement {
    std::string speaker;
    std::string text;
};

struct DebateOption {
    std::string optionId;
    std::string label;
};

struct DebateMenu {
    std::string menuId;
    int statementIndex = -1;
    std::string spanId;
    std::vector<DebateOption> options;
};

struct DebateDef {
    std::vector<DebateStatement> statements;
    std::vector<DebateMenu> menus;
};

} // namespace Salt2D::Game::Story

#endif // GAME_STORY_RESOURCES_DEBATEDEF_H
