// Game/Presentation/TrialPresentation.cpp
#include "TrialPresentation.h"
#include "Game/Session/StorySession.h"
#include "Render/Passes/CardPass.h"

#include <DirectXMath.h>

namespace Salt2D::Game::Presentation {

using namespace DirectX;

void TrialPresentation::Initialize() {
    camera_.SetPosition({0.0f, 1.4f, 0.0f});
    camera_.SetFovY(60.0f * XM_PI / 180.0f);
    camera_.SetClip(0.1f, 100.0f);

    stage_.Initialize(catalog_);
    director_.Initialize(&camera_);

    UnbindChapter();
}

void TrialPresentation::BindChapter(const RHI::DX11::DX11Device& device,
    const Flow::ChapterDef& chapter, Session::StorySession& session
) {
    UnbindChapter();

    tables_ = &session.Tables();

    stage_.BindTables(tables_);
    stage_.LoadStage(device, chapter.stageId);

    director_.Bind(&stage_, tables_);
    director_.Reset();

    bound_ = true;
}

void TrialPresentation::UnbindChapter() {
    tables_ = nullptr;
    bound_ = false;

    stage_.UnbindTables();
    director_.Unbind();
    director_.Reset();
}

void TrialPresentation::Tick(const Story::StoryPlayer& player, const Core::FrameTime& ft) {
    if (!bound_) return;
    director_.Tick(player, ft);
}

void TrialPresentation::EmitSceneDraw(Render::DrawList& drawList, uint32_t canvasW, uint32_t canvasH) {
    if (!bound_) return;
    stage_.EmitBackground(drawList, canvasW, canvasH);
}

void TrialPresentation::BuildScenePasses(Render::RenderPlan& plan, const Render::DrawList& /*drawList*/) {
    using namespace Render;
    if (!bound_) return;

    auto cards = stage_.Cards();
    if (cards.empty()) return;

    auto pCard = std::make_unique<Render::CardPass>("Scene_Cards", Target::Scene, DepthMode::RW, BlendMode::Alpha, cards);
    plan.passes.push_back(std::move(pCard));
}

void TrialPresentation::FillFrameBlackboard(Render::FrameBlackboard& frame, uint32_t canvasW, uint32_t canvasH) {
    if (!bound_) { frame = Render::DefaultFrameBlackboard(); return; }

    frame.view     = camera_.GetView();
    frame.proj     = camera_.GetProj(canvasW, canvasH);
    frame.viewProj = XMMatrixMultiply(frame.view, frame.proj);

    frame.sceneCrossfade = director_.SceneCrossfade();
    frame.lockPrevScene  = director_.LockPrevScene() ? 1 : 0;
}

} // namespace Salt2D::Game::Presentation
