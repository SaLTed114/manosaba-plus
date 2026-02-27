// Game/Session/StoryScreenManager.cpp
#include "StoryScreenManager.h"
#include "Game/Story/StoryPlayer.h"

namespace Salt2D::Game::Session {

void StoryScreenManager::Initialize() {
    theme_.InitDefault();
    themeInited_ = true;

    vn_.     SetTheme(&theme_);
    debate_. SetTheme(&theme_);
    present_.SetTheme(&theme_);
    choice_. SetTheme(&theme_);

    overlay_.SetTheme(&theme_);
}

void StoryScreenManager::Bind(const StorySessionBindings& bindings) {
    Unbind();
    
    player_ = bindings.player;
    tables_ = bindings.tables;

    vn_.     SetPlayer(player_);
    debate_. SetPlayer(player_);
    present_.SetPlayer(player_);
    choice_. SetPlayer(player_);

    vn_.     SetTables(tables_);
    debate_. SetTables(tables_);

    overlay_.SetPlayer(player_);
    overlay_.SetHistory(bindings.history);

    lastType_ = Story::NodeType::Unknown;
}

void StoryScreenManager::Unbind() {
    if (active_) { active_->OnExit(); active_ = nullptr; }

    player_ = nullptr;
    tables_ = nullptr;
    lastType_ = Story::NodeType::Unknown;

    vn_.     SetPlayer(nullptr);
    debate_. SetPlayer(nullptr);
    present_.SetPlayer(nullptr);
    choice_. SetPlayer(nullptr);

    vn_.     SetTables(nullptr);
    debate_. SetTables(nullptr);

    overlay_.SetPlayer(nullptr);
    overlay_.SetHistory(nullptr);
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

    const auto& type0 = player_->View().nodeType;
    SwitchTo(type0, canvasW, canvasH);

    auto af = actionMap_.Map(type0, in);
    overlay_.Tick(ft, af, canvasW, canvasH);
    if (active_) active_->Tick(af, canvasW, canvasH);

    const auto& type1 = player_->View().nodeType;
    if (type1 != type0) SwitchTo(type1, canvasW, canvasH);
}

void StoryScreenManager::Bake(const RHI::DX11::DX11Device& device, RenderBridge::TextService& service) {
    if (active_) active_->Bake(device, service);
    overlay_.Bake(device, service);
}

void StoryScreenManager::PostBake(const Core::InputState& in, uint32_t canvasW, uint32_t canvasH) {
    if (!player_) return;

    auto af = actionMap_.Map(player_->View().nodeType, in);
    overlay_.PostBake(af, canvasW, canvasH);
    if (active_) active_->PostBake(af, canvasW, canvasH);
}

void StoryScreenManager::EmitDraw(Render::DrawList& drawList, RenderBridge::TextureService& service) {
    if (active_) active_->EmitDraw(drawList, service);
    overlay_.EmitDraw(drawList, service);
}

} // namespace Salt2D::Game::Session
