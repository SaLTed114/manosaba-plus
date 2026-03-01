// Game/Story/Runners/NovelSceneState.h
#ifndef GAME_STORY_RUNNERS_NOVELSCENESTATE_H
#define GAME_STORY_RUNNERS_NOVELSCENESTATE_H

#include <string>
#include <optional>
#include <vector>
#include <algorithm>

#include "Game/Story/Resources/VnScript.h"

namespace Salt2D::Game::Story {

struct NovelBgState {
    std::string bgId;
    TransitionSpec transition;
};

struct NovelActorState {
    std::string castId;
    bool visible = false;

    std::string slot;
    std::string expr;

    TransitionSpec transition;
};

struct NovelSceneState {
    NovelBgState bg;
    std::vector<NovelActorState> actors;

    void Reset() {
        bg = NovelBgState{};
        actors.clear();
    }

    NovelActorState* FindActor(const std::string& castId) {
        auto it = std::find_if(actors.begin(), actors.end(),
            [&castId](const NovelActorState& actor) { return actor.castId == castId; });
        return (it != actors.end()) ? &(*it) : nullptr;
    }
    const NovelActorState* FindActor(const std::string& castId) const {
        auto it = std::find_if(actors.begin(), actors.end(),
            [&castId](const NovelActorState& actor) { return actor.castId == castId; });
        return (it != actors.end()) ? &(*it) : nullptr;
    }

    NovelActorState& GetOrCreateActor(const std::string& castId) {
        if (auto* actor = FindActor(castId)) return *actor;
        actors.push_back(NovelActorState{ .castId = castId });
        return actors.back();
    }

    void ApplyBgCmd(const VnCmd::BgCmd& cmd) {
        bg.bgId = cmd.bgId;
        bg.transition = cmd.transition;
    }
    void ApplyCharShowCmd(const VnCmd::CharShowCmd& cmd) {
        auto& actor = GetOrCreateActor(cmd.castId);
        actor.visible = true;
        if (cmd.slot.has_value()) actor.slot = cmd.slot.value();
        else if (actor.slot.empty()) actor.slot = "default";
        if (cmd.expr.has_value()) actor.expr = cmd.expr.value();
        actor.transition = cmd.transition;
    }
    void ApplyCharHideCmd(const VnCmd::CharHideCmd& cmd) {
        auto* actor = FindActor(cmd.castId);
        if (!actor) return;
        actor->visible = false;
        actor->transition = cmd.transition;
    }
    void ApplyCharMoveCmd(const VnCmd::CharMoveCmd& cmd) {
        auto& actor = GetOrCreateActor(cmd.castId);
        actor.slot = cmd.slot;
        actor.transition = cmd.transition;
    }
    void ApplyCharExprCmd(const VnCmd::CharExprCmd& cmd) {
        auto& actor = GetOrCreateActor(cmd.castId);
        actor.expr = cmd.expr;
        actor.transition = cmd.transition;
    }

    void ApplyCmd(const VnCmd& cmd) {
        switch (cmd.type) {
        case VnCmdType::BG:
            if (cmd.bg) ApplyBgCmd(*cmd.bg);
            break;
        case VnCmdType::CharShow:
            if (cmd.charShow) ApplyCharShowCmd(*cmd.charShow);
            break;
        case VnCmdType::CharHide:
            if (cmd.charHide) ApplyCharHideCmd(*cmd.charHide);
            break;
        case VnCmdType::CharMove:
            if (cmd.charMove) ApplyCharMoveCmd(*cmd.charMove);
            break;
        case VnCmdType::CharExpr:
            if (cmd.charExpr) ApplyCharExprCmd(*cmd.charExpr);
            break;
        case VnCmdType::Line:
        default:
            break;
        }
    }

    void ClearPendingTransitions() {
        bg.transition = TransitionSpec::MakeDefault();
        for (auto& actor : actors) {
            actor.transition = TransitionSpec::MakeDefault();
        }
    }
};

} // namespace Salt2D::Game::Story

#endif // GAME_STORY_RUNNERS_NOVELSCENESTATE_H
