// Game/Screens/VnScreen.cpp
#include "VnScreen.h"

#include "Game/Session/StoryActions.h"
#include "Game/Session/StoryHistory.h"
#include "Game/Story/StoryPlayer.h"
#include "Render/Draw/DrawList.h"
#include "Utils/MathUtils.h"

#include <Windows.h>

namespace Salt2D::Game::Screens {

void VnScreen::HandleKeyboard(Session::ActionFrame& af) {
    if (!player_) return;
    if (player_->HistoryOpened()) return;

    const auto& view = player_->View().vn;
    if (!view.has_value()) { dialog_.SetVisible(false); auto_.SetVisible(false); return; }

    const auto accel = af.actions.accelHolded;
    const auto confirm = af.actions.ConsumeConfirm();

    if (accel) {
        player_->FastForward();
        LogHistory();
        return;
    }

    if (confirm) {
        player_->Advance();
        LogHistory();
        return;
    }
}

void VnScreen::HandlePointer(Session::ActionFrame& af) {
    if (!player_) return;
    if (player_->HistoryOpened()) return;

    const auto interaction = UI::UIInteraction::Update(frame_, af, pointer_);

    if (auto_.Visible()) {
        auto_.ApplyHover(frame_, interaction.hovered);
        
        if (auto_.TryToggle(interaction.clicked)) {
            player_->ToggleVnAutoMode();
            return;
        }
    }

    if (af.pointer.lPressed && dialog_.Visible() && interaction.hovered == 0) {
        player_->Advance();
        LogHistory();
    }
}

void VnScreen::BuildUI(uint32_t canvasW, uint32_t canvasH) {
    frame_.Clear();
    dialog_.SetVisible(false);
    auto_.SetVisible(false);

    if (!player_) return;
    const auto& view = player_->View().vn;
    if (!view.has_value()) { dialog_.SetVisible(false); auto_.SetVisible(false); return; }

    UI::VnHudModel model;
    model.visible = true;
    model.speakerUtf8 = view->speaker;
    model.bodyUtf8 = view->fullText;
    model.autoMode = player_->VnAutoMode();

    if (view->totalCp > 0) {
        model.bodyRevealU01 = view->revealCpF / static_cast<float>(view->totalCp);
        model.bodyRevealU01 = Utils::Clamp01(model.bodyRevealU01);
    }

    if (tables_) {
        const auto& castDef = tables_->cast.FindByName(view->speaker);
        if (castDef) {
            model.color.r = castDef->textColor.r;
            model.color.g = castDef->textColor.g;
            model.color.b = castDef->textColor.b;
            model.color.a = castDef->textColor.a;
        }
    }

    dialog_.Build(model, canvasW, canvasH, frame_);
    auto_.Build(model, canvasW, canvasH, frame_);
}

void VnScreen::Tick(Session::ActionFrame& af, uint32_t canvasW, uint32_t canvasH) {
    if (!player_) { frame_.Clear(); dialog_.SetVisible(false); auto_.SetVisible(false); return; }

    HandleKeyboard(af);
    BuildUI(canvasW, canvasH);
}

void VnScreen::Bake(const RHI::DX11::DX11Device& device, RenderBridge::TextService& service) {
    if (!player_) return;
    if (!dialog_.Visible()) return;
    if (!theme_) return;

    baker_.Bake(device, service, frame_);
    dialog_.AfterBake(frame_);
    auto_.AfterBake(frame_);
}

void VnScreen::PostBake(Session::ActionFrame& af, uint32_t /*canvasW*/, uint32_t /*canvasH*/) {
    if (!player_) return;
    if (!dialog_.Visible() && !auto_.Visible()) return;

    HandlePointer(af);
}

void VnScreen::EmitDraw(Render::DrawList& drawList, RenderBridge::TextureService& service) {
    if (!player_) return;
    if (!dialog_.Visible() && !auto_.Visible()) return;

    emitter_.Emit(drawList, service, frame_);
}

void VnScreen::LogHistory() {
    if (!player_ || !history_) return;
    const auto& view = player_->View().vn;
    if (!view.has_value()) return;

    std::string lineKey = view->speaker + ":" + view->fullText;
    if (lineKey == lastLineKey_) return;

    history_->Push(Story::NodeType::VN, view->speaker, view->fullText);
    lastLineKey_ = std::move(lineKey);
}

void VnScreen::OnEnter() {
    dialog_.SetVisible(false);
    auto_.SetVisible(false);
    baker_.SetTheme(theme_);
    LogHistory();
}

void VnScreen::OnExit() {
    dialog_.SetVisible(false);
    auto_.SetVisible(false);
}

} // namespace Salt2D::Game::Screens
