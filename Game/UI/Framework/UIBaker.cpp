// Game/UI/Framework/UIBaker.cpp
#include "UIBaker.h"
#include <stdexcept>

namespace Salt2D::Game::UI {

void UIBaker::Bake(const RHI::DX11::DX11Device& device,
    RenderBridge::TextService& service, UIFrame& frame
) {
    if (!theme_) throw std::runtime_error("UIBaker: theme not set");

    for (auto& text : frame.texts) {
        const auto& style = theme_->GetStyle(text.styleId);

        text.baked = service.GetOrBake(device,
            static_cast<uint8_t>(text.styleId),
            style, text.textUtf8,
            text.layoutW, text.layoutH);
    }
}

} // namespace Salt2D::Game::UI
