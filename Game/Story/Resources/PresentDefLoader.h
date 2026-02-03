// Game/Story/Resources/PresentDefLoader.h
#ifndef GAME_STORY_RESOURCES_PRESENTDEFLOADER_H
#define GAME_STORY_RESOURCES_PRESENTDEFLOADER_H

#include "PresentDef.h"
#include "Utils/IFileSystem.h"
#include <filesystem>

namespace Salt2D::Game::Story {

PresentDef LoadPresentDef(Utils::IFileSystem& fs, const std::filesystem::path& fullPath);

} // namespace Salt2D::Game::Story

#endif // GAME_STORY_RESOURCES_PRESENTDEFLOADER_H
