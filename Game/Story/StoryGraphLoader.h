// Game/Story/StoryGraphLoader.h
#ifndef GAME_STORY_STORYGRAPHLOADER_H
#define GAME_STORY_STORYGRAPHLOADER_H

#include <filesystem>

#include "StoryGraph.h"
#include "Utils/IFileSystem.h"

namespace Salt2D::Game::Story {

struct StoryGraphLoadOptions {
    bool checkResourcesExists = true;
    bool requireDebateNoCommit = true;
    bool requireDebateTimeDepleted = true;
};

StoryGraph LoadStoryGraph(
    Utils::IFileSystem& fs,
    const std::filesystem::path& graphFullPath,
    const StoryGraphLoadOptions& opt = {}
);

} // namespace Salt2D::Game::Story

#endif // GAME_STORY_STORYGRAPHLOADER_H
