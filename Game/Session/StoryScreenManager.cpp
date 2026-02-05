// Game/Session/StoryScreenManager.cpp
#include "StoryScreenManager.h"
#include "Game/Story/StoryPlayer.h"

namespace Salt2D::Game::Session {

void StoryScreenManager::SetPlayer(Story::StoryPlayer* player) {
    player_ = player;
    vn_.SetPlayer(player);
    debate_.SetPlayer(player);
    present_.SetPlayer(player);
}

Screens::IStoryScreen* StoryScreenManager::Pick(Story::NodeType type) {
    switch (type) {
    case Story::NodeType::VN:
    case Story::NodeType::Error:
    case Story::NodeType::BE:
        return &vn_;
    case Story::NodeType::Debate:
        return &debate_;
    case Story::NodeType::Present:
        return &present_;
    default:
        return nullptr;
    }
}

void StoryScreenManager::Tick(const Core::InputState& in, uint32_t canvasW, uint32_t canvasH) {
    if (!player_) return;

    const auto currType = player_->View().nodeType;
    auto* next = Pick(currType);

    if (next != active_) {
        if (active_) active_->OnExit();
        active_ = next;
        if (active_) active_->OnEnter();
    }

    if (active_) active_->Tick(in, canvasW, canvasH);
}

void StoryScreenManager::Bake(const RHI::DX11::DX11Device& device, RenderBridge::TextService& service) {
    if (active_) active_->Bake(device, service);
}

void StoryScreenManager::EmitDraw(Render::DrawList& drawList, ID3D11ShaderResourceView* whiteSRV) {
    if (active_) active_->EmitDraw(drawList, whiteSRV);
}

} // namespace Salt2D::Game::Session
