// Game/Story/Resources/DebateDefLoader.h
#ifndef GAME_STORY_RESOURCES_DEBATEDEFLOADER_H
#define GAME_STORY_RESOURCES_DEBATEDEFLOADER_H

#include "DebateDef.h"
#include "Utils/IFileSystem.h"
#include <filesystem>

namespace Salt2D::Game::Story {

DebateDef LoadDebateDef(Utils::IFileSystem& fs, const std::filesystem::path& fullPath);

} // namespace Salt2D::Game::Story

#endif // GAME_STORY_RESOURCES_DEBATEDEFLOADER_H
