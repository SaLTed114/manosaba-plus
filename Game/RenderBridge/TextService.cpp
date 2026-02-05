// Game/RenderBridge/TextService.cpp
#include "TextService.h"

namespace Salt2D::Game::RenderBridge {

void TextService::Initialize() {
    if (inited_) return;
    baker_.Initialize();
    inited_ = true;
}

void TextService::ClearCache() {
    cache_.Clear();
}

Render::Text::BakedText TextService::GetOrBake(
    const RHI::DX11::DX11Device& device,
    uint8_t styleId,
    const Render::Text::TextStyle& style,
    const std::string& textUtf8,
    float layoutW, float layoutH
) {
    return cache_.GetOrBake(
        device, baker_, styleId, style,
        textUtf8, layoutW, layoutH);
}

} // namespace Salt2D::Game::RenderBridge
