// Game/Screens/StoryOverlayLayer.cpp
#include "StoryOverlayLayer.h"

#include <iostream>

namespace Salt2D::Game::Screens {

void StoryOverlayLayer::HandleKeyboard(Session::ActionFrame& af) {
    if (!player_) return;
    const auto& type = player_->CurrentNode().type;

    const auto history = af.actions.vnHistoryUp;
    const auto cancel = af.actions.ConsumeCancel();

    switch (type) {
    case Story::NodeType::VN:
    case Story::NodeType::BE:
    case Story::NodeType::Error:
    case Story::NodeType::Choice:
        if (history && !player_->HistoryOpened()) {
            player_->OpenHistory();
            historyLogger_->DumpToLogger();
            return;
        }
        if (cancel && player_->HistoryOpened()) {
            player_->CloseHistory();
        }
        break;
    case Story::NodeType::Present:
    case Story::NodeType::Debate:
    default:
        break;
    }
}

void StoryOverlayLayer::HandlePointer(Session::ActionFrame& af) {
    if (!player_) return;
    const auto& type = player_->CurrentNode().type;

    const auto iteraction = UI::UIInteraction::Update(frame_, af, pointer_);

    switch (type) {
    case Story::NodeType::VN:
    case Story::NodeType::BE:
    case Story::NodeType::Error:
    case Story::NodeType::Choice:
        if (history_.Visible()) {
            history_.ApplyHover(frame_, iteraction.hovered);

            if (history_.TryClose(iteraction.clicked)) {
                player_->CloseHistory();
            }
        }
    case Story::NodeType::Present:
    case Story::NodeType::Debate:
    default:
        break;
    }
}

void StoryOverlayLayer::BuildUI(uint32_t canvasW, uint32_t canvasH) {
    frame_.Clear();
    timer_.SetVisible(false);
    history_.SetVisible(false);

    if (!player_) return;

    const auto& view = player_->View().timer;
    timer_.Build(view, canvasW, canvasH, frame_);

    UI::HistoryModel model;
    model.active = player_->HistoryOpened();
    if (historyLogger_) model.entries = historyLogger_->GetEntries();
    
    history_.Build(model, canvasW, canvasH, frame_);
}

void StoryOverlayLayer::Tick(const Core::FrameTime& /*ft*/, Session::ActionFrame& af, uint32_t canvasW, uint32_t canvasH) {
    if (!player_) { frame_.Clear(); timer_.SetVisible(false); history_.SetVisible(false); return; }

    HandleKeyboard(af);
    BuildUI(canvasW, canvasH);
}

void StoryOverlayLayer::Bake(const RHI::DX11::DX11Device& device, RenderBridge::TextService& service) {
    if (!player_) return;
    if (!theme_) return;

    baker_.Bake(device, service, frame_);
    timer_.AfterBake(frame_);
    history_.AfterBake(frame_);
}

void StoryOverlayLayer::PostBake(Session::ActionFrame& af, uint32_t /*canvasW*/, uint32_t /*canvasH*/) {
    if (!player_) return;

    HandlePointer(af);
}

void StoryOverlayLayer::EmitDraw(Render::DrawList& drawList, RenderBridge::TextureService& service) {
    if (!player_) return;

    emitter_.Emit(drawList, service, frame_);
}

} // namespace Salt2D::Game::Screens
