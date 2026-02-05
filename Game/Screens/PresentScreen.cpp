// Game/Screens/PresentScreen.cpp
#include "PresentScreen.h"

#include "Game/Session/StoryActions.h"
#include "Game/Story/StoryPlayer.h"
#include "Game/RenderBridge/TextService.h"
#include "Render/Draw/DrawList.h"

#include <Windows.h>

namespace Salt2D::Game::Screens {

int PresentScreen::ClampWarp(int v, int n) {
    if (n <= 0) return 0;
    v %= n;
    if (v < 0) v += n;
    return v;
}

void PresentScreen::EnsureStyles() {
    if (stylesInited_) return;

    promptStyle_.fontFamily = L"SimSun";
    promptStyle_.fontSize = 20.0f;
    promptStyle_.weight = DWRITE_FONT_WEIGHT_REGULAR;

    itemStyle_.fontFamily = L"SimSun";
    itemStyle_.fontSize = 18.0f;
    itemStyle_.weight = DWRITE_FONT_WEIGHT_REGULAR;

    stylesInited_ = true;
}

void PresentScreen::HandleInput(Session::ActionFrame& af) {
    const auto& view = player_->View().present;
    if (!view.has_value()) { draw_.visible = false; return; }

    const int itemCount = static_cast<int>(view->items.size());
    if (itemCount == 0) { draw_.visible = false; return; }

    const auto navY = af.actions.ConsumeNavY();
    const bool confirm = af.actions.ConsumeConfirm();

    if (navY < 0) --selectedItem_;
    if (navY > 0) ++selectedItem_;

    selectedItem_ = ClampWarp(selectedItem_, itemCount);

    if (!confirm) return;

    const auto& itemId = view->items[selectedItem_].first;
    player_->PickEvidence(itemId);
}

void PresentScreen::BuildUI(uint32_t canvasW, uint32_t canvasH) {
    const auto& view = player_->View().present;
    if (!view.has_value()) { draw_.visible = false; return; }

    Game::UI::PresentHudModel model;
    model.visible    = true;
    model.promptUtf8 = view->prompt;
    model.items      = view->items;

    model.selectedItem = ClampWarp(selectedItem_, static_cast<int>(view->items.size()));

    draw_ = hud_.Build(model, canvasW, canvasH);
}

void PresentScreen::Tick(Session::ActionFrame& af, uint32_t canvasW, uint32_t canvasH) {
    if (!player_) return;
    EnsureStyles();

    HandleInput(af);
    BuildUI(canvasW, canvasH);
}

void PresentScreen::Bake(const RHI::DX11::DX11Device& device, RenderBridge::TextService& service) {
    if (!player_) return;
    if (!draw_.visible) return;

    promptText_ = service.GetOrBake(device,
        static_cast<uint8_t>(draw_.prompt.style),
        promptStyle_,
        draw_.prompt.textUtf8,
        draw_.prompt.layoutW,
        draw_.prompt.layoutH);

    itemTexts_.clear();
    itemTexts_.reserve(draw_.items.size());
    for (const auto& itemReq : draw_.items) {
        auto baked = service.GetOrBake(device,
            static_cast<uint8_t>(itemReq.style),
            itemStyle_,
            itemReq.textUtf8,
            itemReq.layoutW,
            itemReq.layoutH);
        itemTexts_.push_back(std::move(baked));
    }
}

void PresentScreen::EmitDraw(Render::DrawList& drawList, ID3D11ShaderResourceView* whiteSRV) {
    if (!draw_.visible) return;

    // Present panel background
    {
        drawList.PushSprite(Render::Layer::HUD, whiteSRV,
            draw_.panel, 0.0f, {}, draw_.panelTint);
    }

    // Prompt
    if (promptText_.tex.SRV()) {
        Render::RectF dst{
            draw_.prompt.x, draw_.prompt.y,
            static_cast<float>(promptText_.w),
            static_cast<float>(promptText_.h)};
        drawList.PushSprite(Render::Layer::HUD, promptText_.tex.SRV(), dst);
    }

    // Items
    for (size_t i = 0; i < itemTexts_.size(); ++i) {
        const auto& itemText = itemTexts_[i];
        if (!itemText.tex.SRV()) continue;

        const auto& itemReq = draw_.items[i];
        Render::RectF dst{
            itemReq.x, itemReq.y,
            static_cast<float>(itemText.w),
            static_cast<float>(itemText.h)};
        drawList.PushSprite(Render::Layer::HUD, itemText.tex.SRV(), dst);
    }

    // Highlights
    for (const auto& hlReq : draw_.highlightRects) {
        drawList.PushSprite(Render::Layer::HUD, whiteSRV,
            hlReq, 0.0f, {}, draw_.highlightTint);
    }
}

void PresentScreen::OnEnter() {
    selectedItem_ = 0;

    draw_.visible = false;
    promptText_ = {};
    itemTexts_.clear();
}


void PresentScreen::OnExit() {
    draw_.visible = false;
    promptText_ = {};
    itemTexts_.clear();
}

} // namespace Salt2D::Game::Screens
