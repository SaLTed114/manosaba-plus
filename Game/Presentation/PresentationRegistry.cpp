// Game/Presentation/PresentationRegistry.cpp
#include "PresentationRegistry.h"
#include "NullPresentation.h"
#include "TrialPresentation.h"
#include "Game/RenderBridge/TextureCatalog.h"
#include "Render/RenderPlan.h"

namespace Salt2D::Game::Presentation {

PresentationRegistry::~PresentationRegistry() { OnSessionCleared(); }

IChapterPresentation& PresentationRegistry::Active() {
    auto& slot = slots_[ToIndex(activeKey_)];
    if (slot) return *slot;

    // Fallback to null presentation
    auto& nullSlot = slots_[ToIndex(PresKey::Null)];
    if (!nullSlot) {
        nullSlot = std::make_unique<NullPresentation>();
        nullSlot->Initialize();
        activeKey_ = PresKey::Null;
    }
    return *nullSlot;
}

void PresentationRegistry::SetActive(PresKey key) {
    activeKey_ = key;
    if (!slots_[ToIndex(key)]) activeKey_ = PresKey::Null;
}

void PresentationRegistry::Initialize(RenderBridge::TextureCatalog& catalog) {
    slots_[ToIndex(PresKey::Novel)] = std::make_unique<NullPresentation>(); // tmp placeholder
    slots_[ToIndex(PresKey::Trial)] = std::make_unique<TrialPresentation>(catalog);
    slots_[ToIndex(PresKey::Null)]  = std::make_unique<NullPresentation>();

    for (auto& slot : slots_) {
        if (slot) slot->Initialize();
    }

    activeKey_ = PresKey::Null;
    bound_ = false;
}

void PresentationRegistry::OnSessionCleared() {
    if (bound_) { Active().UnbindChapter(); bound_ = false; }
    activeKey_ = PresKey::Null;
}

void PresentationRegistry::OnFinished() {
    OnSessionCleared();
}

void PresentationRegistry::OnSessionCreated(
    const RHI::DX11::DX11Device& device,
    const Flow::ChapterDef& chapter,
    Session::StorySession& session
) {
    if (bound_) { Active().UnbindChapter(); bound_ = false; }

    SetActive(ToKey(chapter.kind));

    auto& active = Active();
    active.BindChapter(device, chapter, session);
    bound_ = true;
}

void PresentationRegistry::Tick(const Story::StoryPlayer& player, const Core::FrameTime& ft) {
    if (!bound_) return;
    Active().Tick(player, ft);
}

void PresentationRegistry::EmitSceneDraw(Render::DrawList& drawList, uint32_t canvasW, uint32_t canvasH) {
    Active().EmitSceneDraw(drawList, canvasW, canvasH);
}

void PresentationRegistry::BuildScenePasses(Render::RenderPlan& plan, const Render::DrawList& drawList) {
    Active().BuildScenePasses(plan, drawList);
}

void PresentationRegistry::FillFrameBlackboard(Render::FrameBlackboard& frame, uint32_t canvasW, uint32_t canvasH) {
    Active().FillFrameBlackboard(frame, canvasW, canvasH);
}

} // namespace Salt2D::Game::Presentation
