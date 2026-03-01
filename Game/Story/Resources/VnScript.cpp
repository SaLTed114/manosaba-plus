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

static bool TryString(const json& j, const char* key, std::string& out) {
    if (!j.contains(key)) return false;
    if (!j[key].is_string()) return false;
    out = j[key].get<std::string>();
    return true;
}

static bool TryString(const json& j, const char* key, std::optional<std::string>& out) {
    out = std::nullopt;
    if (!j.contains(key)) return false;
    if (!j[key].is_string()) return false; 
    out = j[key].get<std::string>();
    return true;
}

[[maybe_unused]] static bool TryFloat(const json& j, const char* key, float& out) {
    if (!j.contains(key)) return false;
    if (!j[key].is_number()) return false;
    out = j[key].get<float>();
    return true;
}

static bool TryFloat(const json& j, const char* key, std::optional<float>& out) {
    out = std::nullopt;
    if (!j.contains(key)) return false; 
    if (!j[key].is_number()) return false;
    out = j[key].get<float>();
    return true;
}

static TrasitionType ParseTransitionType(const std::string& str, const std::string& context) {
    if (str == "default") return TrasitionType::Default;
    if (str == "cut")     return TrasitionType::Cut;
    if (str == "fade")    return TrasitionType::Fade;
    if (str == "slide")   return TrasitionType::Slide;
    throw std::runtime_error("VnScriptLoader: invalid transition type '" + str + "' in " + context);
}

static void TryTransition(const json& j, const char* key, TransitionSpec& out) {
    out = TransitionSpec::MakeDefault();
    if (!j.contains(key)) return;
    if (!j[key].is_string()) return;
    out.type = ParseTransitionType(j[key].get<std::string>(), "transition");
    TryFloat(j, "duration", out.duration);
    out.Normalize();
}

static VnCmdType ParseCmdType(const std::string& str, const std::string& context) {
    if (str == "line")      return VnCmdType::Line;
    if (str == "bg")        return VnCmdType::BG;
    if (str == "char_show") return VnCmdType::CharShow;
    if (str == "char_hide") return VnCmdType::CharHide;
    if (str == "char_move") return VnCmdType::CharMove;
    if (str == "char_expr") return VnCmdType::CharExpr;
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
        cmd.type = ParseCmdType(RequireString(jCmd, "type", ctx), ctx);
        switch (cmd.type) {
        case VnCmdType::Line: {
            VnCmd::LineCmd lineCmd;
            lineCmd.speaker = RequireString(jCmd, "speaker", ctx);
            lineCmd.text = RequireString(jCmd, "text", ctx);
            TryString(jCmd, "perf_id", lineCmd.perfId);
            TryString(jCmd, "expr", lineCmd.expr);

            cmd.line = std::move(lineCmd);
            break;
        }
        case VnCmdType::BG: {
            VnCmd::BgCmd bgCmd;
            bgCmd.bgId = RequireString(jCmd, "bg_id", ctx);
            TryTransition(jCmd, "transition", bgCmd.transition);

            cmd.bg = std::move(bgCmd);
            break;
        }
        case VnCmdType::CharShow: {
            VnCmd::CharShowCmd charShowCmd;
            charShowCmd.castId = RequireString(jCmd, "cast_id", ctx);
            TryString(jCmd, "slot", charShowCmd.slot);
            TryString(jCmd, "expr", charShowCmd.expr);
            TryTransition(jCmd, "transition", charShowCmd.transition);

            cmd.charShow = std::move(charShowCmd);
            break;
        }
        case VnCmdType::CharHide: {
            VnCmd::CharHideCmd charHideCmd;
            charHideCmd.castId = RequireString(jCmd, "cast_id", ctx);
            TryTransition(jCmd, "transition", charHideCmd.transition);

            cmd.charHide = std::move(charHideCmd);
            break;
        }
        case VnCmdType::CharMove: {
            VnCmd::CharMoveCmd charMoveCmd;
            charMoveCmd.castId = RequireString(jCmd, "cast_id", ctx);
            charMoveCmd.slot = RequireString(jCmd, "slot", ctx);
            TryTransition(jCmd, "transition", charMoveCmd.transition);

            cmd.charMove = std::move(charMoveCmd);
            break;
        }
        case VnCmdType::CharExpr: {
            VnCmd::CharExprCmd charExprCmd;
            charExprCmd.castId = RequireString(jCmd, "cast_id", ctx);
            charExprCmd.expr = RequireString(jCmd, "expr", ctx);
            TryTransition(jCmd, "transition", charExprCmd.transition);

            cmd.charExpr = std::move(charExprCmd);
            break;
        }
        }

        script.cmds.push_back(std::move(cmd));
    }

    return script;
}

} // namespace Salt2D::Game::Story
