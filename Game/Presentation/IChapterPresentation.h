// Game/Presentation/IChapterPresentation.h
#ifndef GAME_PRESENTATION_ICHAPTERPRESENTATION_H
#define GAME_PRESENTATION_ICHAPTERPRESENTATION_H

#include <cstdint>

#include "Core/Time/FrameClock.h"
#include "Render/Draw/DrawList.h"
#include "Render/RenderPlan.h"
#include "Render/DX11Renderer.h"
#include "Game/Flow/GameFlowTypes.h"

namespace Salt2D::RHI::DX11 {
    class DX11Device;
} // namespace Salt2D::RHI::DX11

namespace Salt2D::Game::Story {
    class StoryPlayer;
    struct StoryTables;
} // namespace Salt2D::Game::Story

namespace Salt2D::Game::Session {
    class StorySession;
} // namespace Salt2D::Game::Session

namespace Salt2D::Game::Presentation {

class IChapterPresentation {
public:
    virtual ~IChapterPresentation() = default;

    virtual void Initialize() = 0;

    virtual void BindChapter(const RHI::DX11::DX11Device& device,
        const Flow::ChapterDef& chapter, Session::StorySession& session) = 0;

    virtual void UnbindChapter() = 0;

    virtual void Tick(const Story::StoryPlayer& player, const Core::FrameTime& ft) = 0;

    // Push 2D elements to draw list
    virtual void EmitSceneDraw(Render::DrawList& drawList, uint32_t canvasW, uint32_t canvasH) = 0;

    // Build render passes for the current scene
    virtual void BuildScenePasses(Render::RenderPlan& plan, const Render::DrawList& drawList) = 0;

    virtual void FillFrameBlackboard(Render::FrameBlackboard& frame, uint32_t canvasW, uint32_t canvasH) = 0;
};

} // namespace Salt2D::Game::Presentation

#endif // GAME_PRESENTATION_ICHAPTERPRESENTATION_H
