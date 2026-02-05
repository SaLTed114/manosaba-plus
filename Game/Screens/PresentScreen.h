// Game/Screens/PresentScreen.h
#ifndef GAME_SCREENS_PRESENTSCREEN_H
#define GAME_SCREENS_PRESENTSCREEN_H

#include "IStoryScreen.h"

#include "Game/Story/StoryView.h"
#include "Game/UI/PresentHud.h"
#include "Render/Text/TextBaker.h"

#include <vector>

namespace Salt2D::Game::Screens {

class PresentScreen final : public IStoryScreen {
public:
    void SetPlayer(Story::StoryPlayer* player) override { player_ = player; }

    void Tick(const Core::InputState& in, uint32_t canvasW, uint32_t canvasH) override;
    void Bake(const RHI::DX11::DX11Device& device, RenderBridge::TextService& service) override;
    void EmitDraw(Render::DrawList& drawList, ID3D11ShaderResourceView* whiteSRV) override;

    bool Visible() const { return draw_.visible; }

    void OnEnter() override;
    void OnExit()  override;

private:
    static int ClampWarp(int v, int n);

    void EnsureStyles();
    void HandleInput(const Core::InputState& in);
    void BuildUI(uint32_t canvasW, uint32_t canvasH);

private:
    Story::StoryPlayer* player_ = nullptr;

    UI::PresentHud hud_;
    UI::PresentHudDrawData draw_;

    int selectedItem_ = 0;

    Render::Text::TextStyle promptStyle_;
    Render::Text::TextStyle itemStyle_;

    Render::Text::BakedText promptText_;
    std::vector<Render::Text::BakedText> itemTexts_;

    bool stylesInited_ = false;
};

} // namespace Salt2D::Game::Screens

#endif // GAME_SCREENS_PRESENTSCREEN_H
