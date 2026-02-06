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

void ChoiceScreen::EnsureStyles() {
    if (stylesInited_) return;

    optionStyle_.fontFamily = L"SimSun";
    optionStyle_.fontSize = 18.0f;
    optionStyle_.weight = DWRITE_FONT_WEIGHT_REGULAR;

    stylesInited_ = true;
}

void ChoiceScreen::HandleInput(Session::ActionFrame& af) {
    const auto& view = player_->View().choice;
    if (!view.has_value()) { draw_.visible = false; return; }

    const int optionCount = static_cast<int>(view->options.size());
    if (optionCount == 0) { draw_.visible = false; return; }

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

void ChoiceScreen::BuildUI(uint32_t canvasW, uint32_t canvasH) {
    const auto& view = player_->View().choice;
    if (!view.has_value()) { draw_.visible = false; return; }
    
    Game::UI::ChoiceHudModel model;
    model.visible = true;
    model.options = view->options;

    model.selectedOption = ClampWarp(selectedOption_, static_cast<int>(view->options.size()));

    draw_ = hud_.Build(model, canvasW, canvasH);
}

void ChoiceScreen::Tick(Session::ActionFrame& af, uint32_t canvasW, uint32_t canvasH) {
    if (!player_) return;
    EnsureStyles();

    HandleInput(af);
    BuildUI(canvasW, canvasH);
}

void ChoiceScreen::Bake(const RHI::DX11::DX11Device& device, RenderBridge::TextService& service) {
    if (!player_) return;
    if (!draw_.visible) return;

    optionTexts_.clear();
    optionTexts_.reserve(draw_.options.size());
    for (const auto& optionReq : draw_.options) {
        auto baked = service.GetOrBake(device,
            static_cast<uint8_t>(optionReq.style),
            optionStyle_,
            optionReq.textUtf8,
            optionReq.layoutW,
            optionReq.layoutH);
        optionTexts_.push_back(std::move(baked));
    }
}

void ChoiceScreen::EmitDraw(Render::DrawList& drawList, RenderBridge::TextureService& service) {
    if (!draw_.visible) return;

    // Panel background
    {
        drawList.PushSprite(Render::Layer::HUD, service.Get(UI::TextureId::White),
            draw_.panel, 0.0f, {}, draw_.panelTint);
    }

    // Options
    for (size_t i = 0; i < optionTexts_.size(); ++i) {
        const auto& optionText = optionTexts_[i];
        if (!optionText.tex.SRV()) continue;

        const auto& optionReq = draw_.options[i];
        Render::RectF dst{
            optionReq.x, optionReq.y,
            static_cast<float>(optionText.w),
            static_cast<float>(optionText.h)};
        drawList.PushSprite(Render::Layer::HUD, optionText.tex.SRV(), dst);
    }

    // Highlights
    for (const auto& hlReq : draw_.highlightRects) {
        drawList.PushSprite(Render::Layer::HUD, service.Get(UI::TextureId::White),
            hlReq, 0.0f, {}, draw_.highlightTint);
    }
}

void ChoiceScreen::OnEnter() {
    selectedOption_ = 0;

    draw_.visible = false;
    optionTexts_.clear();

    const auto& view = player_->View().choice;
    if (!view.has_value()) return;
    std::string line;
    for (const auto& [id, label] : view->options) line += "| " + label;
    if (history_) history_->Push(Story::NodeType::Choice,
        Session::HistoryKind::OptionList, "", line, "");
}

void ChoiceScreen::OnExit() {
    draw_.visible = false;
    optionTexts_.clear();
}

} // namespace Salt2D::Game::Screens
