// Game/Presentation/TrialPresentation.h
#ifndef GAME_PRESENTATION_TRIALPRESENTATION_H
#define GAME_PRESENTATION_TRIALPRESENTATION_H

#include "IChapterPresentation.h"

#include "Game/Director/StageWorld.h"
#include "Game/Director/StageCameraDirector.h"
#include "Game/RenderBridge/TextureCatalog.h"
#include "Render/Scene3D/Camera3D.h"

namespace Salt2D::Game::Presentation {

class TrialPresentation final : public IChapterPresentation {
public:
    explicit TrialPresentation(RenderBridge::TextureCatalog& catalog)
        : catalog_(&catalog) {}

    void Initialize() override;

    void BindChapter(const RHI::DX11::DX11Device& device,
        const Flow::ChapterDef& chapter, Session::StorySession& session) override;

    void UnbindChapter() override;

    void Tick(const Story::StoryPlayer& player, const Core::FrameTime& ft) override;

    void EmitSceneDraw(Render::DrawList& drawList, uint32_t canvasW, uint32_t canvasH) override;

    void BuildScenePasses(Render::RenderPlan& plan, const Render::DrawList& drawList) override;

    void FillFrameBlackboard(Render::FrameBlackboard& frame, uint32_t canvasW, uint32_t canvasH) override;

private:
    RenderBridge::TextureCatalog* catalog_ = nullptr;
    const Story::StoryTables* tables_ = nullptr;
    bool bound_ = false;

    Director::StageWorld stage_;
    Director::StageCameraDirector director_;
    Render::Scene3D::Camera3D camera_;
};

} // namespace Salt2D::Game::Presentation

#endif // GAME_PRESENTATION_TRIALPRESENTATION_H
