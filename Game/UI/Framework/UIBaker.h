// Game/UI/Framework/UIBaker.h
#ifndef GAME_UI_FRAMEWORK_UIBAKER_H
#define GAME_UI_FRAMEWORK_UIBAKER_H

#include "UIFrame.h"
#include "Game/RenderBridge/TextService.h"
#include "Render/Text/TextBaker.h"
#include "RHI/DX11/DX11Device.h"

namespace Salt2D::Game::UI {

class UIBaker {
public:
    void Bake(const RHI::DX11::DX11Device& device,
        RenderBridge::TextService& service,
        const Render::Text::TextStyle& style,
        UIFrame& frame);
};

} // namespace Salt2D::Game::UI

#endif // GAME_UI_FRAMEWORK_UIBAKER_H
