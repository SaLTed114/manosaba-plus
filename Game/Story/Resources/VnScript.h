// Game/Story/Resources/VnScript.h
#ifndef GAME_STORY_RESOURCES_VNSCRIPT_H
#define GAME_STORY_RESOURCES_VNSCRIPT_H

#include <string>
#include <vector>
#include <filesystem>
#include <optional>
#include "Utils/IFileSystem.h"

namespace Salt2D::Game::Story {

enum class TrasitionType {
    Default,
    Cut,
    Fade,
    Slide,
};

struct TransitionSpec {
    TrasitionType type = TrasitionType::Default;
    std::optional<float> duration;

    void Normalize() {
        switch (type) {
        case TrasitionType::Default:
        case TrasitionType::Cut:
            duration.reset();
            break;
        case TrasitionType::Fade:
        case TrasitionType::Slide:
            break;
        default:
            type = TrasitionType::Default;
            duration.reset();
            break;
        }
    }

    bool UsesTime() const {
        return type == TrasitionType::Fade || type == TrasitionType::Slide;
    }

    bool IsExplicit() const {
        return type != TrasitionType::Default;
    }

    static TransitionSpec MakeDefault() { return TransitionSpec{}; }
    static TransitionSpec MakeCut() { return TransitionSpec{ .type = TrasitionType::Cut }; }
};

enum class VnCmdType {
    Line,
    BG,
    CharShow,
    CharHide,
    CharMove,
    CharExpr,
};

struct VnCmd {
    VnCmdType type = VnCmdType::Line;

    struct LineCmd {
        std::string speaker;   // For Line
        std::string text;      // For Line
        std::string perfId;    // For Line, optional
        std::optional<std::string> expr;
    };
    std::optional<LineCmd> line;

    struct BgCmd {
        std::string bgId;     // For BG
        TransitionSpec transition;
    };
    std::optional<BgCmd> bg;

    struct CharShowCmd {
        std::string castId;   // For CharShow
        std::optional<std::string> slot;     // For CharShow: optional, if not specified, use current slot of the cast
        std::optional<std::string> expr;     // For CharShow: optional, expression id, e.g. "happy", "sad", etc.
        TransitionSpec transition;
    };
    std::optional<CharShowCmd> charShow;

     struct CharHideCmd {
        std::string castId;   // For CharHide
        TransitionSpec transition;
    };
    std::optional<CharHideCmd> charHide;

    struct CharMoveCmd {
        std::string castId;   // For CharMove
        std::string slot;     // For CharMove
        TransitionSpec transition;
    };
    std::optional<CharMoveCmd> charMove;

    struct CharExprCmd {
        std::string castId;   // For CharExpr
        std::string expr;     // For CharExpr: expression id, e.g. "happy", "sad", etc.
        TransitionSpec transition;
    };
    std::optional<CharExprCmd> charExpr;
};

struct VnScript {
    std::vector<VnCmd> cmds;
};

VnScript VnScriptLoader(Utils::IFileSystem& fs, const std::filesystem::path& fullPath);

} // namespace Salt2D::Game::Story

#endif // GAME_STORY_RESOURCES_VNSCRIPT_H
