// Game/Screens/ChoiceScreen.cpp
#include "ChoiceScreen.h"

#include "Game/Session/StoryActions.h"
#include "Game/Session/StoryHistory.h"
#include "Game/Story/StoryPlayer.h"
#include "Game/RenderBridge/TextService.h"
#include "Game/RenderBridge/TextureService.h"
#include "Render/Draw/DrawList.h"

#include <Windows.h>

namespace Salt2D::Game::Screens {

int ChoiceScreen::ClampWarp(int v, int n) {
    if (n <= 0) return 0;
    v %= n;
    if (v < 0) v += n;
    return v;
}

void ChoiceScreen::HandleKeyboard(Session::ActionFrame& af) {
    const auto& view = player_->View().choice;
    if (!view.has_value()) { dialog_.SetVisible(false); return; }

    const int optionCount = static_cast<int>(view->options.size());
    if (optionCount == 0) { dialog_.SetVisible(false); return; }

    const auto navY = af.actions.ConsumeNavY();
    const bool confirm = af.actions.ConsumeConfirm();

    if (navY < 0) --selectedOption_;
    if (navY > 0) ++selectedOption_;

    selectedOption_ = ClampWarp(selectedOption_, optionCount);

    if (!confirm) return;

    const std::string optionId = view->options[selectedOption_].first;
    const std::string optionLabel = view->options[selectedOption_].second;
    player_->CommitOption(optionId);
    if (history_) history_->Push(Story::NodeType::Choice,
        Session::HistoryKind::OptionPick, "", optionLabel, optionId);
}

void ChoiceScreen::HandlePointer(Session::ActionFrame& af) {
    if (!dialog_.Visible()) return;
    const auto iteraction = UI::UIInteraction::Update(frame_, af, pointer_);
    dialog_.ApplyHover(frame_, iteraction.hovered);

    int idx = -1;
    if (dialog_.TryCommit(iteraction.clicked, idx)) {
        const auto& view = player_->View().choice;
        if (!view.has_value()) return;
        if (idx < 0 || idx >= static_cast<int>(view->options.size())) return;

        const std::string optionId = view->options[idx].first;
        const std::string optionLabel = view->options[idx].second;
        player_->CommitOption(optionId);
        if (history_) history_->Push(Story::NodeType::Choice,
            Session::HistoryKind::OptionPick, "", optionLabel, optionId);
    }
}

void ChoiceScreen::BuildUI(uint32_t canvasW, uint32_t canvasH) {
    frame_.Clear();
    dialog_.SetVisible(false);

    if (!player_) return;
    const auto& view = player_->View().choice;
    if (!view.has_value()) { dialog_.SetVisible(false); return; }
    
    Game::UI::ChoiceHudModel model;
    model.visible = true;
    model.options = view->options;

    model.selectedOption = ClampWarp(selectedOption_, static_cast<int>(view->options.size()));

    dialog_.Build(model, canvasW, canvasH, frame_);
}

void ChoiceScreen::Tick(Session::ActionFrame& af, uint32_t canvasW, uint32_t canvasH) {
    if (!player_) { frame_.Clear(); dialog_.SetVisible(false); return; }

    if (kbEnabled_) HandleKeyboard(af);
    BuildUI(canvasW, canvasH);
    HandlePointer(af);
}

void ChoiceScreen::Bake(const RHI::DX11::DX11Device& device, RenderBridge::TextService& service) {
    if (!player_) return;
    if (!dialog_.Visible()) return;
    if (!theme_) return;

    baker_.Bake(device, service, frame_);
    dialog_.AfterBake(frame_);
}

void ChoiceScreen::EmitDraw(Render::DrawList& drawList, RenderBridge::TextureService& service) {
    if (!player_) return;
    if (!dialog_.Visible()) return;

    emitter_.Emit(drawList, service, frame_);
}

void ChoiceScreen::OnEnter() {
    selectedOption_ = 0;
    pointer_ = {};
    dialog_.SetVisible(false);
    baker_.SetTheme(theme_);

    const auto& view = player_->View().choice;
    if (!view.has_value()) return;
    std::string line;
    for (const auto& [id, label] : view->options) line += "| " + label;
    if (history_) history_->Push(Story::NodeType::Choice,
        Session::HistoryKind::OptionList, "", line, "");
}

void ChoiceScreen::OnExit() {
    dialog_.SetVisible(false);
}

} // namespace Salt2D::Game::Screens
