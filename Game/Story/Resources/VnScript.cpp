// Game/Story/Resources/VnScript.cpp
#include "VnScript.h"
#include "Utils/FileUtils.h"

#include <nlohmann/json.hpp>
#include <sstream>

namespace Salt2D::Game::Story {

using json = nlohmann::json;

static std::string RequireString(const json& j, const char* key, const std::string& context) {
    if (!j.contains(key) || !j[key].is_string()) {
        throw std::runtime_error("VnScriptLoader: missing/invalid string field '" + std::string(key) + "' in " + context);
    }
    return j[key].get<std::string>();
}

static VnCmdType ParseCmdType(const std::string& str, const std::string& context) {
    if (str == "line") return VnCmdType::Line;
    if (str == "cue")  return VnCmdType::Cue;
    throw std::runtime_error("VnScriptLoader: invalid cmd type '" + str + "' in " + context);
}

VnScript VnScriptLoader(Utils::IFileSystem& fs, const std::filesystem::path& fullPath) {
    if (!fs.Exists(fullPath)) {
        throw std::runtime_error("VnScriptLoader: file does not exist: " + fullPath.string());
    }
    const std::string text = fs.ReadTextFileUtf8(fullPath, true);

    json root = json::parse(text);
    if (!root.is_object()) {
        throw std::runtime_error("VnScriptLoader: root JSON is not an object in " + fullPath.string());
    }

    VnScript script;
    if (!root.contains("cmds") || !root["cmds"].is_array()) {
        throw std::runtime_error("VnScriptLoader: missing/invalid 'cmds' array in " + fullPath.string());
    }
    for (size_t i = 0; i < root["cmds"].size(); i++) {
        const auto& jCmd = root["cmds"][i];
        const auto ctx = "cmds[" + std::to_string(i) + "]";
        if (!jCmd.is_object()) {
            throw std::runtime_error("VnScriptLoader: " + ctx + " must be object");
        }

        VnCmd cmd;
        // tmp not supporting cues in json format, only line cmds for now
        cmd.type = ParseCmdType(RequireString(jCmd, "type", ctx), ctx);
        if (cmd.type == VnCmdType::Line) {
            cmd.speaker = RequireString(jCmd, "speaker", ctx);
            cmd.text = RequireString(jCmd, "text", ctx);
        }

        script.cmds.push_back(std::move(cmd));
    }

    return script;
}

} // namespace Salt2D::Game::Story
