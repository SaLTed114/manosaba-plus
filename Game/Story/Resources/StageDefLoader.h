// Game/Story/Resources/StageDefLoader.h
#ifndef GAME_STORY_RESOURCES_STAGEDEFLOADER_H
#define GAME_STORY_RESOURCES_STAGEDEFLOADER_H

#include "StageDef.h"
#include "Utils/IFileSystem.h"
#include <filesystem>

namespace Salt2D::Game::Story {

StageTable LoadStageTable(Utils::IFileSystem& fs, const std::filesystem::path& fullPath);

} // namespace Salt2D::Game::Story

#endif // GAME_STORY_RESOURCES_STAGEDEFLOADER_H
