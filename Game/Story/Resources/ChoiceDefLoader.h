// Game/Story/Resources/ChoiceDefLoader.h
#ifndef GAME_STORY_RESOURCES_CHOICEDEFLOADER_H
#define GAME_STORY_RESOURCES_CHOICEDEFLOADER_H

#include "ChoiceDef.h"
#include "Utils/IFileSystem.h"
#include <filesystem>

namespace Salt2D::Game::Story {

ChoiceDef LoadChoiceDef(Utils::IFileSystem& fs, const std::filesystem::path& fullPath);

} // namespace Salt2D::Game::Story

#endif // GAME_STORY_RESOURCES_CHOICEDEFLOADER_H
