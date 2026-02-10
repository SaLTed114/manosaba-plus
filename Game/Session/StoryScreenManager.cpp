// Game/Session/StoryScreenManager.cpp
#include "StoryScreenManager.h"
#include "Game/Story/StoryPlayer.h"

namespace Salt2D::Game::Session {

void StoryScreenManager::Initialize(Story::StoryPlayer* player, StoryHistory* history, Story::StoryTables* tables) {
    player_  = player;
    history_ = history;
    tables_  = tables;

    vn_.     SetPlayer(player);
    debate_. SetPlayer(player);
    present_.SetPlayer(player);
    choice_. SetPlayer(player);

    vn_.     SetHistory(history);
    debate_. SetHistory(history);
    present_.SetHistory(history);
    choice_. SetHistory(history);

    vn_.     SetTables(tables);
    debate_. SetTables(tables);

    theme_.InitDefault();
    themeInited_ = true;

    vn_.     SetTheme(&theme_);
    debate_. SetTheme(&theme_);
    present_.SetTheme(&theme_);
    choice_. SetTheme(&theme_);

    overlay_.SetPlayer(player);
    overlay_.SetTheme(&theme_);
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
    case Story::NodeType::Choice:
        return &choice_;
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

void StoryScreenManager::Tick(const Core::FrameTime& ft, const Core::InputState& in, uint32_t canvasW, uint32_t canvasH) {
    if (!player_) return;

    player_->Tick(ft.dtSec);

    const auto& type0 = player_->View().nodeType;
    SwitchTo(type0, canvasW, canvasH);

    auto af = actionMap_.Map(type0, in);
    if (active_) active_->Tick(af, canvasW, canvasH);

    const auto& type1 = player_->View().nodeType;
    if (type1 != type0) SwitchTo(type1, canvasW, canvasH);

    overlay_.Tick(ft, in, canvasW, canvasH);
}

void StoryScreenManager::Bake(const RHI::DX11::DX11Device& device, RenderBridge::TextService& service) {
    if (active_) active_->Bake(device, service);
    overlay_.Bake(device, service);
}

void StoryScreenManager::PostBake(const Core::InputState& in, uint32_t canvasW, uint32_t canvasH) {
    if (!player_) return;

    overlay_.PostBake(in, canvasW, canvasH);

    auto af = actionMap_.Map(player_->View().nodeType, in);
    if (active_) active_->PostBake(af, canvasW, canvasH);
}

void StoryScreenManager::EmitDraw(Render::DrawList& drawList, RenderBridge::TextureService& service) {
    if (active_) active_->EmitDraw(drawList, service);
    overlay_.EmitDraw(drawList, service);
}

} // namespace Salt2D::Game::Session
