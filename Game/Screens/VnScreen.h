// Game/Screens/VnScreen.h
#ifndef GAME_SCREENS_VNSCREEN_H
#define GAME_SCREENS_VNSCREEN_H

#include "IStoryScreen.h"

#include "Game/UI/Widgets/VnDialogWidget.h"
#include "Game/UI/Framework/UIFrame.h"
#include "Game/UI/Framework/UIBaker.h"
#include "Game/UI/Framework/UIEmitter.h"
#include "Game/UI/Theme/TextTheme.h"
#include "Render/Text/TextBaker.h"

namespace Salt2D::Game::Screens {

class VnScreen final : public IStoryScreen {
public:
    void SetPlayer(Story::StoryPlayer* player) override { player_ = player; }
    void SetHistory(Session::StoryHistory* history) override { history_ = history; }
    void SetTheme(UI::TextTheme* theme) { theme_ = theme; baker_.SetTheme(theme); }

    void Tick(Session::ActionFrame& af, uint32_t canvasW, uint32_t canvasH) override;
    void Sync(uint32_t canvasW, uint32_t canvasH) override { BuildUI(canvasW, canvasH); }
    void Bake(const RHI::DX11::DX11Device& device, RenderBridge::TextService& service) override;
    void EmitDraw(Render::DrawList& drawList, RenderBridge::TextureService& service) override;

    bool Visible() const { return dialog_.Visible(); }

    void OnEnter() override;
    void OnExit()  override;

private:
    void HandleInput(Session::ActionFrame& af);
    void BuildUI(uint32_t canvasW, uint32_t canvasH);
    void LogHistory();

private:
    Story::StoryPlayer* player_ = nullptr;
    Session::StoryHistory* history_ = nullptr;
    UI::TextTheme* theme_ = nullptr;

    std::string lastLineKey_;
    bool logOpened_ = false;

    UI::UIFrame   frame_;
    UI::UIBaker   baker_;
    UI::UIEmitter emitter_;

    UI::VnDialogWidget dialog_;
};

} // namespace Salt2D::Game::Screens

#endif // GAME_SCREENS_VNSCREEN_H
