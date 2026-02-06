// Game/UI/Framework/UIBaker.cpp
#include "UIBaker.h"

namespace Salt2D::Game::UI {

void UIBaker::Bake(const RHI::DX11::DX11Device& device,
    RenderBridge::TextService& service,
    const Render::Text::TextStyle& style,
    UIFrame& frame)
{
    for (auto& text : frame.texts) {
        text.baked = service.GetOrBake(device,
            static_cast<uint8_t>(text.styleId),
            style, text.textUtf8,
            text.layoutW, text.layoutH);
    }
}

} // namespace Salt2D::Game::UI
