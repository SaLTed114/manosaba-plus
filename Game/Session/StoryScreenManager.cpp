// Game/Session/StoryScreenManager.cpp
#include "StoryScreenManager.h"
#include "Game/Story/StoryPlayer.h"

namespace Salt2D::Game::Session {

void StoryScreenManager::Initialize(Story::StoryPlayer* player, StoryHistory* history) {
    player_ = player;
    history_ = history;

    vn_.SetPlayer(player);
    debate_.SetPlayer(player);
    present_.SetPlayer(player);

    vn_.SetHistory(history);
    debate_.SetHistory(history);
    present_.SetHistory(history);
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

void StoryScreenManager::SwitchTo(Story::NodeType type, uint32_t canvasW, uint32_t canvasH) {
    auto* next = Pick(type);
    if (next != active_) {
        if (active_) active_->OnExit();
        active_ = next;
        if (active_) active_->OnEnter();
        if (active_) active_->Sync(canvasW, canvasH);
    }
}

void StoryScreenManager::Tick(const Core::InputState& in, uint32_t canvasW, uint32_t canvasH) {
    if (!player_) return;

    const auto& type0 = player_->View().nodeType;
    SwitchTo(type0, canvasW, canvasH);

    auto af = actionMap_.Map(type0, in);
    if (active_) active_->Tick(af, canvasW, canvasH);

    const auto& type1 = player_->View().nodeType;
    if (type1 != type0) SwitchTo(type1, canvasW, canvasH);
}

void StoryScreenManager::Bake(const RHI::DX11::DX11Device& device, RenderBridge::TextService& service) {
    if (active_) active_->Bake(device, service);
}

void StoryScreenManager::EmitDraw(Render::DrawList& drawList, ID3D11ShaderResourceView* whiteSRV) {
    if (active_) active_->EmitDraw(drawList, whiteSRV);
}

} // namespace Salt2D::Game::Session
