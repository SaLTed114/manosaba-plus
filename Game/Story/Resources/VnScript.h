// Game/Story/Resources/VnScript.h
#ifndef GAME_STORY_RESOURCES_VNSCRIPT_H
#define GAME_STORY_RESOURCES_VNSCRIPT_H

#include <string>
#include <vector>
#include <filesystem>

namespace Salt2D::Game::Story {

enum class VnCmdType { Line, Cue, };

struct VnCmd {
    VnCmdType type = VnCmdType::Line;
    std::string speaker;   // For Line
    std::string text;      // For Line
    std::string cueName;   // For Cue
};

struct VnScript {
    std::vector<VnCmd> cmds;
};

VnScript ParseVnScriptText(const std::string& utf8Text);

} // namespace Salt2D::Game::Story

#endif // GAME_STORY_RESOURCES_VNSCRIPT_H
