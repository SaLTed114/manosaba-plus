// Game/Story/Resources/VnScript.h
#ifndef GAME_STORY_RESOURCES_VNSCRIPT_H
#define GAME_STORY_RESOURCES_VNSCRIPT_H

#include <string>
#include <vector>
#include <filesystem>
#include "Utils/IFileSystem.h"

namespace Salt2D::Game::Story {

enum class VnCmdType { Line, };

struct VnCmd {
    VnCmdType type = VnCmdType::Line;
    std::string speaker;   // For Line
    std::string text;      // For Line
    std::string perfId;    // For Line, optional
};

struct VnScript {
    std::vector<VnCmd> cmds;
};

VnScript VnScriptLoader(Utils::IFileSystem& fs, const std::filesystem::path& fullPath);

} // namespace Salt2D::Game::Story

#endif // GAME_STORY_RESOURCES_VNSCRIPT_H
