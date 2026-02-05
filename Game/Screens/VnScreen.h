// Game/Screens/VnScreen.h
#ifndef GAME_SCREENS_VNSCREEN_H
#define GAME_SCREENS_VNSCREEN_H

#include "IStoryScreen.h"

#include "Game/UI/VnHud.h"
#include "Render/Text/TextBaker.h"

namespace Salt2D::Game::Screens {

class VnScreen final : public IStoryScreen {
public:
    void SetPlayer(Story::StoryPlayer* player) override { player_ = player; }

    void Tick(const Core::InputState& in, uint32_t canvasW, uint32_t canvasH) override;
    void Bake(const RHI::DX11::DX11Device& device, RenderBridge::TextService& service) override;
    void EmitDraw(Render::DrawList& drawList, ID3D11ShaderResourceView* whiteSRV) override;

    bool Visible() const { return draw_.visible; }

    void OnEnter() override;
    void OnExit()  override;

private:
    void EnsureStyles();
    void HandleInput(const Core::InputState& in);
    void BuildUI(uint32_t canvasW, uint32_t canvasH);

private:
    Story::StoryPlayer* player_ = nullptr;

    UI::VnHud hud_;
    UI::VnHudDrawData draw_;

    Render::Text::TextStyle speakerStyle_;
    Render::Text::TextStyle bodyStyle_;

    Render::Text::BakedText speakerText_;
    Render::Text::BakedText bodyText_;

    bool stylesInited_ = false;
};

} // namespace Salt2D::Game::Screens

#endif // GAME_SCREENS_VNSCREEN_H
