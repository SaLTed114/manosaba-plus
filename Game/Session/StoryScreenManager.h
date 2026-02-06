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
#include "Game/Story/StoryTypes.h"
#include "Game/UI/Theme/TextTheme.h"

namespace Salt2D::Game::Session {

class StoryScreenManager {
public:
    void Initialize(Story::StoryPlayer* player, StoryHistory* history);

    void Tick(const Core::InputState& in, uint32_t canvasW, uint32_t canvasH);
    void Bake(const RHI::DX11::DX11Device& device, RenderBridge::TextService& service);
    void EmitDraw(Render::DrawList& drawList, ID3D11ShaderResourceView* whiteSRV);

private:
    Screens::IStoryScreen* Pick(Story::NodeType type);
    void SwitchTo(Story::NodeType type, uint32_t canvasW, uint32_t canvasH);

private:
    Story::StoryPlayer* player_  = nullptr;
    StoryHistory*       history_ = nullptr;
    Story::NodeType lastType_ = Story::NodeType::Unknown;

    UI::TextTheme theme_;
    bool themeInited_ = false;

    StoryActionMap actionMap_;

    Screens::VnScreen       vn_;
    Screens::DebateScreen   debate_;
    Screens::PresentScreen  present_;
    Screens::ChoiceScreen   choice_;

    Screens::IStoryScreen* active_ = nullptr;
};

} // namespace Salt2D::Game::Session

#endif // GAME_SESSION_STORYSCREENMANAGER_H
