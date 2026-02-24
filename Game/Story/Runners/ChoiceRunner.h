// Game/Story/Runners/ChoiceRunner.h
#ifndef GAME_STORY_RUNNERS_CHOICERUNNER_H
#define GAME_STORY_RUNNERS_CHOICERUNNER_H

#include <optional>

#include "Game/Story/StoryTypes.h"
#include "Game/Story/Resources/ChoiceDef.h"
#include "Utils/IFileSystem.h"
#include "Utils/Logger.h"

namespace Salt2D::Game::Story {

class ChoiceRunner {
public:
    explicit ChoiceRunner(Utils::IFileSystem& fs);

    void Enter(const Node& node);

    std::optional<GraphEvent> Choose(const std::string& optionId);

    const ChoiceDef& Def() const { return def_; }

    void SetLogger(const Utils::Logger* logger) { logger_ = logger; }

private:
    Utils::IFileSystem& fs_;
    ChoiceDef def_;
    const Utils::Logger* logger_ = nullptr;
};

} // namespace Salt2D::Game::Story

#endif // GAME_STORY_RUNNERS_CHOICERUNNER_H
