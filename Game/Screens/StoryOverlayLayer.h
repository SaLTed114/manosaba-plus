// Game/Screens/StoryOverlayLayer.h
#ifndef GAME_SCREENS_STORYOVERLAYLAYER_H
#define GAME_SCREENS_STORYOVERLAYLAYER_H

#include "Core/Time/FrameClock.h"
#include "Core/Input/InputState.h"

#include "Game/Story/StoryPlayer.h"
#include "Game/UI/Widgets/TimerWidget.h"
#include "Game/UI/Widgets/HistoryWidget.h"
#include "Game/UI/Framework/UIFrame.h"
#include "Game/UI/Framework/UIBaker.h"
#include "Game/UI/Framework/UIEmitter.h"
#include "Game/UI/Framework/UIInteraction.h"
#include "Game/UI/Theme/TextTheme.h"
#include "Render/Text/TextBaker.h"

namespace Salt2D::Game::Screens {

class StoryOverlayLayer {
public:
    void SetPlayer(Story::StoryPlayer* player) { player_ = player; }
    void SetHistory(Session::StoryHistory* history) { historyLogger_ = history; }
    void SetTheme(UI::TextTheme* theme) { theme_ = theme; baker_.SetTheme(theme); }

    void Tick(const Core::FrameTime& ft, Session::ActionFrame& af, uint32_t canvasW, uint32_t canvasH);
    void Bake(const RHI::DX11::DX11Device& device, RenderBridge::TextService& service);
    void PostBake(Session::ActionFrame& af, uint32_t canvasW, uint32_t canvasH);
    void EmitDraw(Render::DrawList& drawList, RenderBridge::TextureService& service);

private:
    void HandleKeyboard(Session::ActionFrame& af);
    void HandlePointer(Session::ActionFrame& af);
    void BuildUI(uint32_t canvasW, uint32_t canvasH);

private:
    Story::StoryPlayer* player_ = nullptr;
    Session::StoryHistory* historyLogger_ = nullptr;
    UI::TextTheme* theme_ = nullptr;

    UI::UIFrame   frame_;
    UI::UIBaker   baker_;
    UI::UIEmitter emitter_;

    UI::UIPointerState pointer_;
    UI::TimerWidget timer_;
    UI::HistoryWidget history_;

    float scrollY_ = 0.0f;
};

} // namespace Salt2D::Game::Screens

#endif // GAME_SCREENS_STORYOVERLAYLAYER_H
