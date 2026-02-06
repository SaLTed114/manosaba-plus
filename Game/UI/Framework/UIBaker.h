// Game/UI/Framework/UIBaker.h
#ifndef GAME_UI_FRAMEWORK_UIBAKER_H
#define GAME_UI_FRAMEWORK_UIBAKER_H

#include "UIFrame.h"
#include "Game/UI/Theme/TextTheme.h"
#include "Game/RenderBridge/TextService.h"
#include "Render/Text/TextBaker.h"
#include "RHI/DX11/DX11Device.h"

namespace Salt2D::Game::UI {

class UIBaker {
public:
    void SetTheme(const TextTheme* theme) { theme_ = theme; }

    void Bake(const RHI::DX11::DX11Device& device,
        RenderBridge::TextService& service, UIFrame& frame);

private:
    const TextTheme* theme_ = nullptr;
};

} // namespace Salt2D::Game::UI

#endif // GAME_UI_FRAMEWORK_UIBAKER_H
