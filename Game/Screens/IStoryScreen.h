// Game/Screens/IStoryScreen.h
#ifndef GAME_SCREENS_ISTORYSCREEN_H
#define GAME_SCREENS_ISTORYSCREEN_H

#include <cstdint>

struct ID3D11ShaderResourceView;

namespace Salt2D::RHI::DX11 {
    class DX11Device;
} // namespace Salt2D::RHI::DX11

namespace Salt2D::Render {
    class DrawList;
} // namespace Salt2D::Render

namespace Salt2D::Game::Story {
    class StoryPlayer;
} // namespace Salt2D::Game::Story

namespace Salt2D::Game::RenderBridge {
    class TextService;
} // namespace Salt2D::Game::RenderBridge

namespace Salt2D::Game::Session {
    struct ActionFrame;
} // namespace Salt2D::Game::Session

namespace Salt2D::Game::Screens {

class IStoryScreen {
public:
    virtual ~IStoryScreen() = default;

    virtual void SetPlayer(Story::StoryPlayer* player) = 0;

    virtual void Tick(Session::ActionFrame& af, uint32_t canvasW, uint32_t canvasH) = 0;
    virtual void Sync(uint32_t /*canvasW*/, uint32_t /*canvasH*/) {}
    virtual void Bake(const RHI::DX11::DX11Device& device, RenderBridge::TextService& service) = 0;
    virtual void EmitDraw(Render::DrawList& drawList, ID3D11ShaderResourceView* whiteSRV) = 0;

    virtual void OnEnter() = 0;
    virtual void OnExit() = 0;
};

} // namespace Salt2D::Game::Screens

#endif // GAME_SCREENS_ISTORYSCREEN_H
