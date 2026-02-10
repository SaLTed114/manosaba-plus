// Game/Story/Resources/CastDefLoader.h
#ifndef GAME_STORY_RESOURCES_CASTDEFLOADER_H
#define GAME_STORY_RESOURCES_CASTDEFLOADER_H

#include "CastDef.h"
#include "Utils/IFileSystem.h"
#include <filesystem>

namespace Salt2D::Game::Story {

CastTable LoadCastTable(Utils::IFileSystem& fs, const std::filesystem::path& fullPath);

} // namespace Salt2D::Game::Story

#endif // GAME_STORY_RESOURCES_CASTDEFLOADER_H
