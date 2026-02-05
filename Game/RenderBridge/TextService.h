// Game/RenderBridge/TextService.h
#ifndef GAME_RENDERBRIDGE_TEXTSERVICE_H
#define GAME_RENDERBRIDGE_TEXTSERVICE_H

#include <cstdint>
#include <string>

#include "RHI/DX11/DX11Device.h"
#include "Render/Text/TextBaker.h"
#include "Render/Text/TextCache.h"

namespace Salt2D::Game::RenderBridge {

class TextService {
public:
    void Initialize();
    void ClearCache();

    Render::Text::BakedText GetOrBake(
        const RHI::DX11::DX11Device& device,
        uint8_t styleId,
        const Render::Text::TextStyle& style,
        const std::string& textUtf8,
        float layoutW, float layoutH);

private:
    bool inited_ = false;
    Render::Text::TextBaker baker_;
    Render::Text::TextCache cache_;
};

} // namespace Salt2D::Game::RenderBridge

#endif // GAME_RENDERBRIDGE_TEXTSERVICE_H
