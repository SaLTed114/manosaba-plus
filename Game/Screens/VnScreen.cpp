// Game/Screens/VnScreen.cpp
#include "VnScreen.h"

#include "Game/Story/StoryPlayer.h"
#include "Game/RenderBridge/TextService.h"
#include "Render/Draw/DrawList.h"
#include "Core/Input/InputState.h"

#include <Windows.h>

namespace Salt2D::Game::Screens {

void VnScreen::EnsureStyles() {
    if (stylesInited_) return;

    speakerStyle_.fontFamily = L"SimSun";
    speakerStyle_.fontSize = 24.0f;
    speakerStyle_.weight = DWRITE_FONT_WEIGHT_BOLD;

    bodyStyle_.fontFamily = L"SimSun";
    bodyStyle_.fontSize = 20.0f;
    bodyStyle_.weight = DWRITE_FONT_WEIGHT_REGULAR;

    stylesInited_ = true;
}

void VnScreen::HandleInput(const Core::InputState& in) {
    const auto& view = player_->View().vn;
    if (!view.has_value()) { draw_.visible = false; return; }

    if (in.Pressed(VK_RETURN) || in.Pressed(VK_SPACE)) {
        if (in.Down(VK_SHIFT)) player_->FastForward();
        else player_->Advance();
    }
}

void VnScreen::BuildUI(uint32_t canvasW, uint32_t canvasH) {
    const auto& view = player_->View().vn;
    if (!view.has_value()) { draw_.visible = false; return; }

    UI::VnHudModel model;
    model.visible = true;
    model.speakerUtf8 = view->speaker;
    model.bodyUtf8 = view->fullText;

    draw_ = hud_.Build(model, canvasW, canvasH);
}

void VnScreen::Tick(const Core::InputState& in, uint32_t canvasW, uint32_t canvasH) {
    if (!player_) return;
    EnsureStyles();

    HandleInput(in);
    BuildUI(canvasW, canvasH);
}

void VnScreen::Bake(const RHI::DX11::DX11Device& device, RenderBridge::TextService& service) {
    if (!player_) return;
    if (!draw_.visible) return;

    speakerText_ = service.GetOrBake(device,
        static_cast<uint8_t>(draw_.speaker.style),
        speakerStyle_,
        draw_.speaker.textUtf8,
        draw_.speaker.layoutW,
        draw_.speaker.layoutH);

    bodyText_ = service.GetOrBake(device,
        static_cast<uint8_t>(draw_.body.style),
        bodyStyle_,
        draw_.body.textUtf8,
        draw_.body.layoutW,
        draw_.body.layoutH);
}

void VnScreen::EmitDraw(Render::DrawList& drawList, ID3D11ShaderResourceView* whiteSRV) {
    if (!draw_.visible) return;

    // VN panel background
    {
        drawList.PushSprite(Render::Layer::HUD, whiteSRV,
            draw_.panel, 0.0f, {}, draw_.panelTint);
    }

    // Speaker
    if (speakerText_.tex.SRV()) {
        Render::RectF dst{
            draw_.speaker.x, draw_.speaker.y,
            static_cast<float>(speakerText_.w),
            static_cast<float>(speakerText_.h)};
        drawList.PushSprite(Render::Layer::HUD, speakerText_.tex.SRV(), dst);
    }

    // Body
    if (bodyText_.tex.SRV()) {
        Render::RectF dst{
            draw_.body.x, draw_.body.y,
            static_cast<float>(bodyText_.w),
            static_cast<float>(bodyText_.h)};
        drawList.PushSprite(Render::Layer::HUD, bodyText_.tex.SRV(), dst);
    }
}

void VnScreen::OnEnter() {
    draw_.visible = false;
    speakerText_ = {};
    bodyText_ = {};
}

void VnScreen::OnExit() {
    draw_.visible = false;
    speakerText_ = {};
    bodyText_ = {};
}

} // namespace Salt2D::Game::Screens
