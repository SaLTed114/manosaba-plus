// Game/Sceens/DebateScreen.cpp
#include "DebateScreen.h"

#include "Game/Session/StoryActions.h"
#include "Game/Story/StoryPlayer.h"
#include "Game/RenderBridge/TextService.h"
#include "Render/Draw/DrawList.h"

#include <Windows.h>

namespace Salt2D::Game::Screens {

int DebateScreen::ClampWarp(int v, int n) {
    if (n <= 0) return 0;
    v %= n;
    if (v < 0) v += n;
    return v;
}

void DebateScreen::EnsureStyles() {
    if (stylesInited_) return;

    speakerStyle_.fontFamily = L"SimSun";
    speakerStyle_.fontSize = 24.0f;
    speakerStyle_.weight = DWRITE_FONT_WEIGHT_BOLD;

    bodyStyle_.fontFamily = L"SimSun";
    bodyStyle_.fontSize = 20.0f;
    bodyStyle_.weight = DWRITE_FONT_WEIGHT_REGULAR;

    smallStyle_.fontFamily = L"SimSun";
    smallStyle_.fontSize = 16.0f;
    smallStyle_.weight = DWRITE_FONT_WEIGHT_REGULAR;

    stylesInited_ = true;
}

void DebateScreen::HandleInput(Session::ActionFrame& af) {
    const auto& view = player_->View().debate;
    if (!view.has_value()) { draw_.visible = false; return; }

    const auto navY = af.actions.ConsumeNavY();
    const auto confirm = af.actions.ConsumeConfirm();
    const auto cancel = af.actions.ConsumeCancel();
    const auto openMenu = af.actions.debateOpenMenuPressed;

    if (view->menuOpen && cancel) {
        player_->CloseDebateMenu();
        return;
    }

    if (navY < 0) {
        if (view->menuOpen) --selectedOption_;
        else --selectedSpan_;
    }
    if (navY > 0) {
        if (view->menuOpen) ++selectedOption_;
        else ++selectedSpan_;
    }

    if (!confirm && !openMenu) return;

    if (!view->menuOpen) {
        if (!view->spanIds.empty() && openMenu) {
            const int index = ClampWarp(selectedSpan_, static_cast<int>(view->spanIds.size()));
            player_->OpenSuspicion(view->spanIds[index]);
        }

        if (confirm) {
            player_->Advance();
        }
    } else {
        const int index = ClampWarp(selectedOption_, static_cast<int>(view->options.size()));
        if (!view->options.empty() && confirm) {
            player_->CommitOption(view->options[index].first);
        }
    }
}

void DebateScreen::BuildUI(uint32_t canvasW, uint32_t canvasH) {
    const auto& view = player_->View().debate;
    if (!view.has_value()) { draw_.visible = false; return; }

    Game::UI::DebateHudModel model;
    model.visible = true;
    model.speakerUtf8  = view->speaker;
    model.bodyUtf8     = view->fullText;
    model.spanIds      = view->spanIds;
    model.menuOpen     = view->menuOpen;
    model.openedSpanId = view->openedSpanId;
    model.menuOptions  = view->options;

    model.selectedSpan = ClampWarp(selectedSpan_,   static_cast<int>(view->spanIds.size()));
    model.selectedOpt  = ClampWarp(selectedOption_, static_cast<int>(view->options.size()));

    draw_ = hud_.Build(model, canvasW, canvasH);
}

void DebateScreen::Tick(Session::ActionFrame& af, uint32_t canvasW, uint32_t canvasH) {
    if (!player_) return;
    EnsureStyles();

    HandleInput(af);
    BuildUI(canvasW, canvasH);
}

void DebateScreen::Bake(const RHI::DX11::DX11Device& device, RenderBridge::TextService& service) {
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

    spanTexts_.clear();
    spanTexts_.reserve(draw_.spans.size());
    for (const auto& spanReq : draw_.spans) {
        auto baked = service.GetOrBake(device,
            static_cast<uint8_t>(spanReq.style),
            bodyStyle_,
            spanReq.textUtf8,
            spanReq.layoutW,
            spanReq.layoutH);
        spanTexts_.push_back(std::move(baked));
    }

    optionTexts_.clear();
    optionTexts_.reserve(draw_.options.size());
    for (const auto& optReq : draw_.options) {
        auto baked = service.GetOrBake(device,
            static_cast<uint8_t>(optReq.style),
            smallStyle_,
            optReq.textUtf8,
            optReq.layoutW,
            optReq.layoutH);
        optionTexts_.push_back(std::move(baked));
    }
}

void DebateScreen::EmitDraw(Render::DrawList& drawList, ID3D11ShaderResourceView* whiteSRV) {
    if (!draw_.visible) return;

    // Debate panel background
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

    // Spans
    for (size_t i = 0; i < spanTexts_.size(); ++i) {
        const auto& spanText = spanTexts_[i];
        if (!spanText.tex.SRV()) continue;

        const auto& spanReq = draw_.spans[i];
        Render::RectF dst{
            spanReq.x, spanReq.y,
            static_cast<float>(spanText.w),
            static_cast<float>(spanText.h)};
        drawList.PushSprite(Render::Layer::HUD, spanText.tex.SRV(), dst);
    }

    // Options
    for (size_t i = 0; i < optionTexts_.size(); ++i) {
        const auto& optText = optionTexts_[i];
        if (!optText.tex.SRV()) continue;

        const auto& optReq = draw_.options[i];
        Render::RectF dst{
            optReq.x, optReq.y,
            static_cast<float>(optText.w),
            static_cast<float>(optText.h)};
        drawList.PushSprite(Render::Layer::HUD, optText.tex.SRV(), dst);
    }

    // Highlights
    for (const auto& hlReq : draw_.highlightRects) {
        drawList.PushSprite(Render::Layer::HUD, whiteSRV,
            hlReq, 0.0f, {}, draw_.highlightTint);
    }
}

void DebateScreen::OnEnter() {
    selectedSpan_ = 0;
    selectedOption_ = 0;

    draw_.visible = false;
    speakerText_ = {};
    bodyText_ = {};
    spanTexts_.clear();
    optionTexts_.clear();
}
void DebateScreen::OnExit() {
    draw_.visible = false;
    speakerText_ = {};
    bodyText_ = {};
    spanTexts_.clear();
    optionTexts_.clear();
}

} // namespace Salt2D::Game::Screens
