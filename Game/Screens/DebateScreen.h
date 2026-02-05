// Game/Screens/DebateScreen.h
#ifndef GAME_SCREENS_DEBATESCREEN_H
#define GAME_SCREENS_DEBATESCREEN_H

#include "IStoryScreen.h"

#include "Game/Story/StoryView.h"
#include "Game/UI/DebateHud.h"
#include "Render/Text/TextBaker.h"

#include <vector>

namespace Salt2D::Game::Screens {

class DebateScreen final : public IStoryScreen {
public:
    void SetPlayer(Story::StoryPlayer* player) override { player_ = player; }

    void Tick(Session::ActionFrame& af, uint32_t canvasW, uint32_t canvasH) override;
    void Sync(uint32_t canvasW, uint32_t canvasH) override { BuildUI(canvasW, canvasH); }
    void Bake(const RHI::DX11::DX11Device& device, RenderBridge::TextService& service) override;
    void EmitDraw(Render::DrawList& drawList, ID3D11ShaderResourceView* whiteSRV) override;

    bool Visible() const { return draw_.visible; }

    void OnEnter() override;
    void OnExit()  override;

private:
    static int ClampWarp(int v, int n);

    void EnsureStyles();
    void HandleInput(Session::ActionFrame& af);
    void BuildUI(uint32_t canvasW, uint32_t canvasH);

private:
    Story::StoryPlayer* player_ = nullptr;

    UI::DebateHud hud_;
    UI::DebateHudDrawData draw_;

    int selectedSpan_ = 0;
    int selectedOption_ = 0;

    Render::Text::TextStyle speakerStyle_;
    Render::Text::TextStyle bodyStyle_;
    Render::Text::TextStyle smallStyle_;

    Render::Text::BakedText speakerText_;
    Render::Text::BakedText bodyText_;
    std::vector<Render::Text::BakedText> spanTexts_;
    std::vector<Render::Text::BakedText> optionTexts_;

    bool stylesInited_ = false;
};

} // namespace Salt2D::Game::Screens

#endif // GAME_SCREENS_DEBATESCREEN_H
