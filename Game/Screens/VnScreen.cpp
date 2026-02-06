// Game/Screens/VnScreen.cpp
#include "VnScreen.h"

#include "Game/Session/StoryActions.h"
#include "Game/Session/StoryHistory.h"
#include "Game/Story/StoryPlayer.h"
#include "Render/Draw/DrawList.h"

#include <Windows.h>

namespace Salt2D::Game::Screens {

void VnScreen::HandleInput(Session::ActionFrame& af) {
    const auto& view = player_->View().vn;
    if (!view.has_value()) { dialog_.SetVisible(false); return; }

    const auto accel = af.actions.accelHolded;
    const auto history = af.actions.vnHistoryUp;
    const auto confirm = af.actions.ConsumeConfirm();
    const auto cancel = af.actions.ConsumeCancel();

    if (logOpened_) {
        if (cancel) logOpened_ = false;
        return;
    }

    if (accel) {
        player_->FastForward();
        LogHistory();
        return;
    }

    if (history) {
        if (history_) history_->DumpToLogger();
        logOpened_ = true;
        return;
    }

    if (confirm) {
        player_->Advance();
        LogHistory();
        return;
    }
}

void VnScreen::BuildUI(uint32_t canvasW, uint32_t canvasH) {
    frame_.Clear();
    dialog_.SetVisible(false);

    if (!player_) return;
    const auto& view = player_->View().vn;
    if (!view.has_value()) { dialog_.SetVisible(false); return; }

    UI::VnHudModel model;
    model.visible = true;
    model.speakerUtf8 = view->speaker;
    model.bodyUtf8 = view->fullText;

    dialog_.Build(model, canvasW, canvasH, frame_);
}

void VnScreen::Tick(Session::ActionFrame& af, uint32_t canvasW, uint32_t canvasH) {
    if (!player_) { frame_.Clear(); dialog_.SetVisible(false); return; }

    HandleInput(af);
    BuildUI(canvasW, canvasH);
}

void VnScreen::Bake(const RHI::DX11::DX11Device& device, RenderBridge::TextService& service) {
    if (!player_) return;
    if (!dialog_.Visible()) return;
    if (!theme_) return;

    baker_.Bake(device, service, frame_);
}

void VnScreen::EmitDraw(Render::DrawList& drawList, ID3D11ShaderResourceView* whiteSRV) {
    if (!player_) return;
    if (!dialog_.Visible()) return;

    emitter_.Emit(drawList, whiteSRV, frame_);
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
    baker_.SetTheme(theme_);
    LogHistory();
}

void VnScreen::OnExit() {
    dialog_.SetVisible(false);
}

} // namespace Salt2D::Game::Screens
