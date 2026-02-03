// Game/Story/Runners/PresentRunner.h
#ifndef GAME_STORY_RUNNERS_PRESENTRUNNER_H
#define GAME_STORY_RUNNERS_PRESENTRUNNER_H

#include <optional>

#include "Game/Story/StoryTypes.h"
#include "Game/Story/Resources/PresentDef.h"
#include "Utils/IFileSystem.h"

namespace Salt2D::Game::Story {

class PresentRunner {
public:
    explicit PresentRunner(Utils::IFileSystem& fs);

    void Enter(const Node& node);

    std::optional<GraphEvent> Pick(const std::string& itemId);

    const PresentDef& Def() const { return def_; }

private:
    Utils::IFileSystem& fs_;
    PresentDef def_;
};

} // namespace Salt2D::Game::Story

#endif // GAME_STORY_RUNNERS_PRESENTRUNNER_H
