// Game/UI/Framework/UIEmitter.cpp
#include "UIEmitter.h"

namespace Salt2D::Game::UI {

static inline ID3D11ShaderResourceView* ResolveTextureSRV(
    const SpriteOp& sprite,
    RenderBridge::TextureService& service
) {
    if (sprite.srv) return sprite.srv;
    if (auto* srv = service.Get(sprite.texId)) return srv;
    return service.Get(TextureId::White);
}

void UIEmitter::Emit(Render::DrawList& drawList,
    RenderBridge::TextureService& service,
    const UIFrame& frame
) {
    for (const auto& sprite : frame.sprites) {
        Render::SpriteDrawItem& item = drawList.PushSprite(
            sprite.layer,
            ResolveTextureSRV(sprite, service),
            sprite.dst,
            sprite.z,
            sprite.uv,
            sprite.tint
        );
        item.clipEnabled = sprite.clipEnabled;
        item.clipRect = sprite.clipRect;
    }

    for (const auto& text : frame.texts) {
        if (!text.baked.tex.SRV()) continue;
        Render::RectF dst{
            text.x, text.y,
            static_cast<float>(text.baked.w),
            static_cast<float>(text.baked.h)
        };
        drawList.PushSprite(text.layer,
            text.baked.tex.SRV(),
            dst, text.z, {}, text.tint);
    }
}

} // namespace Salt2D::Game::UI
