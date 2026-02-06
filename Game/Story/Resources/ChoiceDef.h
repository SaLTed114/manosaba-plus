// Game/Story/Resources/ChoiceDef.h
#ifndef GAME_STORY_RESOURCES_CHOICEDEF_H
#define GAME_STORY_RESOURCES_CHOICEDEF_H

#include <string>
#include <vector>

namespace Salt2D::Game::Story {

struct ChoiceOption {
    std::string optionId;
    std::string label;
};

struct ChoiceDef {
    std::vector<ChoiceOption> options;
};

} // namespace Salt2D::Game::Story

#endif // GAME_STORY_RESOURCES_CHOICEDEF_H
