// Game/Session/StoryScreenManager.h
#ifndef GAME_SESSION_STORYSCREENMANAGER_H
#define GAME_SESSION_STORYSCREENMANAGER_H

#include "Game/Session/StoryActionMap.h"
#include "Game/Session/StoryHistory.h"
#include "Game/Screens/IStoryScreen.h"
#include "Game/Screens/VnScreen.h"
#include "Game/Screens/DebateScreen.h"
#include "Game/Screens/PresentScreen.h"
#include "Game/Screens/ChoiceScreen.h"
#include "Game/Screens/StoryOverlayLayer.h"
#include "Game/Story/StoryTypes.h"
#include "Game/Story/StoryTables.h"
#include "Game/UI/Theme/TextTheme.h"
#include "Core/Time/FrameClock.h"

namespace Salt2D::Game::Session {

class StoryScreenManager {
public:
    void Initialize(Story::StoryPlayer* player, StoryHistory* history, Story::StoryTables* tables);

    void Tick(const Core::FrameTime& ft, const Core::InputState& in, uint32_t canvasW, uint32_t canvasH);
    void Bake(const RHI::DX11::DX11Device& device, RenderBridge::TextService& service);
    void PostBake(const Core::InputState& in, uint32_t canvasW, uint32_t canvasH);
    void EmitDraw(Render::DrawList& drawList, RenderBridge::TextureService& service);

private:
    Screens::IStoryScreen* Pick(Story::NodeType type);
    void SwitchTo(Story::NodeType type, uint32_t canvasW, uint32_t canvasH);

private:
    Story::StoryPlayer* player_  = nullptr;
    Story::NodeType lastType_ = Story::NodeType::Unknown;

    const Story::StoryTables* tables_ = nullptr;

    UI::TextTheme theme_;
    bool themeInited_ = false;

    StoryActionMap actionMap_;

    Screens::VnScreen       vn_;
    Screens::DebateScreen   debate_;
    Screens::PresentScreen  present_;
    Screens::ChoiceScreen   choice_;

    Screens::IStoryScreen* active_ = nullptr;
    Screens::StoryOverlayLayer overlay_;
};

} // namespace Salt2D::Game::Session

#endif // GAME_SESSION_STORYSCREENMANAGER_H
