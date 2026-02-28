// Game/Presentation/NullPresentation.h
#ifndef GAME_PRESENTATION_NULLPRESENTATION_H
#define GAME_PRESENTATION_NULLPRESENTATION_H

#include "IChapterPresentation.h"

namespace Salt2D::Game::Presentation {

class NullPresentation final : public IChapterPresentation {
public:
    void Initialize() override {}

    void BindChapter(const RHI::DX11::DX11Device& /*device*/,
        const Flow::ChapterDef& /*chapter*/, Session::StorySession& /*session*/) override {}

    void UnbindChapter() override {}

    void Tick(const Story::StoryPlayer& /*player*/, const Core::FrameTime& /*ft*/) override {}

    void EmitSceneDraw(Render::DrawList& /*drawList*/, uint32_t /*canvasW*/, uint32_t /*canvasH*/) override {}

    void BuildScenePasses(Render::RenderPlan& /*plan*/, const Render::DrawList& /*drawList*/) override {}

    void FillFrameBlackboard(Render::FrameBlackboard& frame, uint32_t /*canvasW*/, uint32_t /*canvasH*/) override {
        frame = Render::DefaultFrameBlackboard();
    }
};

} // namespace Salt2D::Game::Presentation

#endif // GAME_PRESENTATION_NULLPRESENTATION_H
