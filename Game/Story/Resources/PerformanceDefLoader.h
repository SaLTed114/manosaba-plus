// Game/Story/Resources/PerformanceDefLoader.h
#ifndef GAME_STORY_RESOURCES_PERFORMANCEDEFLOADER_H
#define GAME_STORY_RESOURCES_PERFORMANCEDEFLOADER_H

#include "PerformanceDef.h"
#include "Utils/IFileSystem.h"
#include <filesystem>

namespace Salt2D::Game::Story {

PerformanceTable LoadPerformanceTable(Utils::IFileSystem& fs, const std::filesystem::path& fullPath);

} // namespace Salt2D::Game::Story

#endif // GAME_STORY_RESOURCES_PERFORMANCEDEFLOADER_H
