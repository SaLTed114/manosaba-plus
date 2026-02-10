// Game/Director/HudEvaluator.h
#ifndef GAME_DIRECTOR_HUDEVALUATOR_H
#define GAME_DIRECTOR_HUDEVALUATOR_H

#include <cstdint>
#include <string_view>

namespace Salt2D::Game::Story {
    struct StoryTables;
} // namespace Salt2D::Game::Story

namespace Salt2D::Game::UI {
    struct HudPose2D;
} // namespace Salt2D::Game::UI

namespace Salt2D::Game::Director {

UI::HudPose2D EvalDebateDialogPose(
    const Story::StoryTables& tables,
    uint32_t canvasW, uint32_t canvasH,
    float totalSec, float remainSec,
    std::string_view prefId);

UI::HudPose2D DefaultDebateDialogPose(uint32_t canvasW, uint32_t canvasH);

} // namespace Salt2D::Game::Director

#endif // GAME_DIRECTOR_HUDEVALUATOR_H
