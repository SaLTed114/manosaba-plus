// Game/Screens/PresentScreen.cpp
#include "PresentScreen.h"

#include "Game/Session/StoryActions.h"
#include "Game/Session/StoryHistory.h"
#include "Game/Story/StoryPlayer.h"
#include "Game/RenderBridge/TextService.h"
#include "Game/RenderBridge/TextureService.h"
#include "Render/Draw/DrawList.h"
#include "Utils/MathUtils.h"

#include <Windows.h>

namespace Salt2D::Game::Screens {

void PresentScreen::PickEvidence() {
    const auto& view = player_->View().present;
    if (!view.has_value()) return;
    selectedItem_ = Utils::ClampWarp(selectedItem_, static_cast<int>(view->items.size()));

    const std::string itemId = view->items[selectedItem_].first;
    const std::string itemLabel = view->items[selectedItem_].second;
    player_->PickEvidence(itemId);
    if (history_) history_->Push(Story::NodeType::Present,
        Session::HistoryKind::PresentPick, "", itemLabel, itemId);
}

void PresentScreen::HandleKeyboard(Session::ActionFrame& af) {
    const auto& view = player_->View().present;
    if (!view.has_value()) { dialog_.SetVisible(false); return; }

    const int itemCount = static_cast<int>(view->items.size());
    if (itemCount == 0) { dialog_.SetVisible(false); return; }

    const auto navY = af.actions.ConsumeNavY();
    const bool confirm = af.actions.ConsumeConfirm();

    if (navY < 0) --selectedItem_;
    if (navY > 0) ++selectedItem_;

    selectedItem_ = Utils::ClampWarp(selectedItem_, itemCount);

    if (!confirm) return;

    PickEvidence();
}

void PresentScreen::BuildUI(uint32_t canvasW, uint32_t canvasH) {
    frame_.Clear();
    dialog_.SetVisible(false);

    if (!player_) return;
    const auto& view = player_->View().present;
    if (!view.has_value()) { dialog_.SetVisible(false); return; }

    Game::UI::PresentHudModel model;
    model.visible    = true;
    model.promptUtf8 = view->prompt;
    model.items      = view->items;

    model.selectedItem = Utils::ClampWarp(selectedItem_, static_cast<int>(view->items.size()));

    dialog_.Build(model, canvasW, canvasH, frame_);
}

void PresentScreen::HandlePointer(Session::ActionFrame& af) {
    if (!dialog_.Visible()) return;
    const auto interaction = UI::UIInteraction::Update(frame_, af, pointer_);
    dialog_.ApplyHover(frame_, interaction.hovered);

    if (UI::HitKeyKind(interaction.clicked) == UI::HitKind::PresentItem) {
        selectedItem_ = static_cast<int>(UI::HitKeyIndex(interaction.clicked));
        return;
    }
    if (UI::HitKeyKind(interaction.clicked) == UI::HitKind::PresentShow) {
        PickEvidence();
    }
}

void PresentScreen::Tick(Session::ActionFrame& af, uint32_t canvasW, uint32_t canvasH) {
    if (!player_) { frame_.Clear(); dialog_.SetVisible(false); return; }

    if (kbEnabled_) HandleKeyboard(af);
    BuildUI(canvasW, canvasH);
    HandlePointer(af);
}

void PresentScreen::Bake(const RHI::DX11::DX11Device& device, RenderBridge::TextService& service) {
    if (!player_) return;
    if (!dialog_.Visible()) return;
    if (!theme_) return;

    baker_.Bake(device, service, frame_);
    dialog_.AfterBake(frame_);
}

void PresentScreen::EmitDraw(Render::DrawList& drawList, RenderBridge::TextureService& service) {
    if (!player_) return;
    if (!dialog_.Visible()) return;

    emitter_.Emit(drawList, service, frame_);
}

void PresentScreen::OnEnter() {
    selectedItem_ = 0;
    pointer_ = {};
    dialog_.SetVisible(false);
    baker_.SetTheme(theme_);

    const auto& view = player_->View().present;
    if (!view.has_value()) return;
    if (history_) history_->Push(Story::NodeType::Present,
        Session::HistoryKind::PresentPrompt, "", view->prompt);
}

void PresentScreen::OnExit() {
    dialog_.SetVisible(false);
}

} // namespace Salt2D::Game::Screens
