// Game/UI/Framework/UIEmitter.cpp
#include "UIEmitter.h"
#include "Utils/MathUtils.h"

namespace Salt2D::Game::UI {

static inline ID3D11ShaderResourceView* ResolveTextureSRV(
    const SpriteOp& sprite,
    RenderBridge::TextureService& service
) {
    if (sprite.srv) return sprite.srv;
    if (auto* srv = service.Get(sprite.texId)) return srv;
    return service.Get(TextureId::White);
}

static inline void EmitSprite(Render::DrawList& drawList,
    RenderBridge::TextureService& service,
    const SpriteOp& sprite
) {
    auto& item = drawList.PushSprite(sprite.layer,
        ResolveTextureSRV(sprite, service),
        sprite.dst, sprite.z, sprite.uv, sprite.tint);

    item.clipEnabled = sprite.clipEnabled;
    item.clipRect    = sprite.clipRect;
}

static inline bool WantVnLineReveal(const TextOp& text) {
    const auto& baked = text.baked;
    return text.revealEnabled &&
        (text.styleId == TextStyleId::VnBody) &&
        (!baked.lineRectsPx.empty());
}

static inline void EmitTextNormal(Render::DrawList& drawList,
    const TextOp& text
) {
    if (!text.baked.tex.SRV()) return;

    Render::RectF dst{
        text.x, text.y,
        static_cast<float>(text.baked.w),
        static_cast<float>(text.baked.h)
    };

    Render::UVRectF uv{0,0,1,1};
    if (text.revealEnabled) {
        const float t = Utils::Clamp01(text.revealU01);
        dst.w *= t;
        uv.u0 = 0.0f; uv.v0 = 0.0f;
        uv.u1 = t;    uv.v1 = 1.0f;
    }

    auto& item = drawList.PushSprite(text.layer,
        text.baked.tex.SRV(), dst, text.z, uv, text.tint);

    item.hasTransform = text.transform.hasTransform;
    item.rotRad = text.transform.rotRad;
    item.scaleX = text.transform.scaleX;
    item.scaleY = text.transform.scaleY;
    item.pivotX = text.transform.pivotX;
    item.pivotY = text.transform.pivotY;
}

static inline void EmitTextVnLineRevealWeighted(
    Render::DrawList& drawList,
    const TextOp& text
) {
    if (!text.baked.tex.SRV()) return;

    const int n = static_cast<int>(text.baked.lineRectsPx.size());
    if (n <= 0) return;

    const float u = Utils::Clamp01(text.revealU01);

    float totalW = 0.0f;
    for (const auto& lr : text.baked.lineRectsPx) totalW += (std::max)(0.0f, lr.w);
    if (totalW <= 0.0f) return;

    float revealW = u * totalW;

    const float texW = static_cast<float>(text.baked.w);
    const float texH = static_cast<float>(text.baked.h);

    const bool hasXform = text.transform.hasTransform;
    const float fullW = static_cast<float>(text.baked.w);
    const float fullH = static_cast<float>(text.baked.h);
    const float globalPivotX = text.x + text.transform.pivotX * fullW;
    const float globalPivotY = text.y + text.transform.pivotY * fullH;

    float accumW = 0.0f;

    // TODO: the strip should be handled in shader for better performance, this is just a quick implementation for the VN line reveal effect
    auto PushStrip = [&](const Render::RectF& dst, const Render::UVRectF& uv, float alphaMul) {
        Render::Color4F tint = text.tint;
        tint.a *= alphaMul;

        auto& item = drawList.PushSprite(
            text.layer, text.baked.tex.SRV(),
            dst, text.z, uv, tint);

        item.hasTransform = hasXform;
        if (hasXform) {
            item.rotRad = text.transform.rotRad;
            item.scaleX = text.transform.scaleX;
            item.scaleY = text.transform.scaleY;

            item.pivotX = (dst.w > 0.0f) ? (globalPivotX - dst.x) / dst.w : 0.0f;
            item.pivotY = (dst.h > 0.0f) ? (globalPivotY - dst.y) / dst.h : 0.0f;
        }
    };

    const float softPx = (std::max)(text.revealSoftPx, 0.0f);
    const int softStep = (std::max)(text.revealSoftStep, 0);

    for (const auto& lr : text.baked.lineRectsPx) {
        const float w = (std::max)(0.0f, lr.w);

        float t = 1.0f;
        if (revealW < accumW) t = 0.0f;
        else if (w > 1e-4f) t = Utils::Clamp01((revealW - accumW) / w);
        else t = 0.0f;

        if (t <= 0.0f) break;
        if (lr.h <= 0.0f || w <= 0.0f) break;

        const float wVisible = Utils::Clamp(w * t, 0.0f, w);
        if (wVisible > 0.0f) {
            Render::RectF dst{
                text.x + lr.x,
                text.y + lr.y,
                wVisible, lr.h
            };
            Render::UVRectF uv{
                lr.x / texW, lr.y / texH,
                (lr.x + wVisible) / texW,
                (lr.y + lr.h) / texH
            };
            PushStrip(dst, uv, 1.0f);
        }
        
        if (t < 1.0f && softPx > 1e-4f && softStep > 0) {
            float wSoft = Utils::Clamp(softPx, 0.0f, w - wVisible);
            if (wSoft > 1e-4f) {
                int steps = (std::min)(softStep, static_cast<int>(std::ceil(wSoft)));
                steps = (std::max)(steps, 1);

                const float sliceW = wSoft / static_cast<float>(steps);
                for (int i = 0; i < steps; ++i) {
                    const float x0 = wVisible + sliceW * static_cast<float>(i);
                    float ww = sliceW;
                    if (x0 + ww > wVisible + wSoft) ww = wVisible + wSoft - x0;
                    if (ww <= 1e-4f) continue;

                    const float u01 = Utils::Clamp01(static_cast<float>(i) / static_cast<float>(steps));
                    const float alphaMul = 1.0f - u01;

                    Render::RectF dst{
                        text.x + lr.x + x0,
                        text.y + lr.y,
                        ww, lr.h
                    };
                    Render::UVRectF uv{
                        (lr.x + x0) / texW, lr.y / texH,
                        (lr.x + x0 + ww) / texW, (lr.y + lr.h) / texH
                    };
                    PushStrip(dst, uv, alphaMul);
                }
            }
        }

        accumW += w;
        if (t < 1.0f) break;
    }
}

void UIEmitter::Emit(Render::DrawList& drawList,
    RenderBridge::TextureService& service,
    const UIFrame& frame
) {
    for (const auto& sprite : frame.sprites) {
        EmitSprite(drawList, service, sprite);
    }

    for (const auto& text : frame.texts) {
        if (WantVnLineReveal(text)) {
            EmitTextVnLineRevealWeighted(drawList, text);
        } else {
            EmitTextNormal(drawList, text);
        }
    }
}

} // namespace Salt2D::Game::UI
