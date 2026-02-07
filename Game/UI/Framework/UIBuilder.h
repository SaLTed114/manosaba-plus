// Game/UI/Framework/UIBuilder.h
#ifndef GAME_UI_FRAMEWORK_UIBUILDER_H
#define GAME_UI_FRAMEWORK_UIBUILDER_H

#include "UIFrame.h"
#include "Game/UI/UITypes.h"
#include "Utils/StringUtils.h"

#include <cstdint>
#include <string>
#include <utility>

namespace Salt2D::Game::UI {

// ==============================
// Basic rect helper
// ==============================

struct RectScale { float x=0, y=0, w=0, h=0; };

inline Render::RectF RectXYWH(float x, float y, float w, float h) {return Render::RectF{x, y, w, h}; }

inline Render::RectF RectFromScale(const RectScale& s, uint32_t W, uint32_t H) {
    return Render::RectF{ s.x * W, s.y * H, s.w * W, s.h * H };
}

// ==============================
// Safe access helpers
// ==============================

inline SpriteOp* GetSprite(UIFrame& frame, int idx) {
    if (idx < 0 || static_cast<size_t>(idx) >= frame.sprites.size()) return nullptr;
    return &frame.sprites[idx];
}

inline TextOp* GetText(UIFrame& frame, int idx) {
    if (idx < 0 || static_cast<size_t>(idx) >= frame.texts.size()) return nullptr;
    return &frame.texts[idx];
}

// =============================
// Push op helpers
// =============================

inline int PushSprite(
    UIFrame& frame, TextureId texId,
    const Render::RectF& dst, const Render::Color4F& tint, float z = 0.0f,
    Render::Layer layer = Render::Layer::HUD,
    ID3D11ShaderResourceView* srv = nullptr
) {
    SpriteOp op;
    op.layer = layer;
    op.texId = texId;
    op.srv = srv;
    op.dst = dst;
    op.tint = tint;
    op.z = z;

    int idx = static_cast<int>(frame.sprites.size());
    frame.sprites.push_back(std::move(op));
    return idx;
}

inline int PushText(
    UIFrame& frame, TextStyleId styleId, std::string textUtf8,
    float x, float y, float layoutW, float layoutH,
    const Render::Color4F& tint, float z = 0.0f,
    Render::Layer layer = Render::Layer::HUD
) {
    TextOp op;
    op.layer = layer;
    op.styleId = styleId;
    op.textUtf8 = std::move(textUtf8);
    op.x = x;
    op.y = y;
    op.layoutW = layoutW;
    op.layoutH = layoutH;
    op.tint = tint;
    op.z = z;

    int idx = static_cast<int>(frame.texts.size());
    frame.texts.push_back(std::move(op));
    return idx;
}

inline int PushTextInRect(
    UIFrame& frame, TextStyleId styleId, std::string textUtf8,
    const Render::RectF& dst, const Render::Color4F& tint, float z = 0.0f,
    Render::Layer layer = Render::Layer::HUD
) {
    return PushText(frame, styleId, std::move(textUtf8),
        dst.x, dst.y, dst.w, dst.h, tint, z, layer);
}

inline int PushHit(
    UIFrame& frame, HitKey key, const Render::RectF& rect,
    bool enabled = true, bool visible = true
) {
    HitOp op;
    op.key = key;
    op.rect = rect;
    op.enabled = enabled;
    op.visible = visible;

    int idx = static_cast<int>(frame.hits.size());
    frame.hits.push_back(std::move(op));
    return idx;
}

// ==============================
// Common combo: button (sprite + text + hit)
// ==============================

struct UIButtonRef {
    HitKey key = 0;
    Render::RectF rect{};
    int sprite = -1;
    int text   = -1;
    int hit    = -1;
};

inline UIButtonRef PushButton(
    UIFrame& frame, HitKey key,
    const Render::RectF& dst, TextureId bgTex, const Render::Color4F& bgTint,
    TextStyleId styleId, std::string textUtf8, const Render::Color4F& textTint,
    float zBg = 0.1f, float zText = 0.2f, Render::Layer layer = Render::Layer::HUD
) {
    UIButtonRef button;
    button.key = key;
    button.rect = dst;
    button.sprite = PushSprite(frame, bgTex, dst, bgTint, zBg, layer);
    button.text   = PushTextInRect(frame, styleId, std::move(textUtf8), dst, textTint, zText, layer);
    button.hit    = PushHit(frame, key, dst, true, true);
    return button;
}

// =============================
// AfterBake helpers
// =============================

inline void CenterTextInRect(UIFrame& frame, int textIdx, const Render::RectF& rect) {
    TextOp* textOp = GetText(frame, textIdx);
    if (!textOp) return;

    textOp->x = rect.x + (rect.w - static_cast<float>(textOp->baked.w)) * 0.5f;
    textOp->y = rect.y + (rect.h - static_cast<float>(textOp->baked.h)) * 0.5f;
}

// =============================
// First Glyph helper
// =============================

struct FirstGlyphTextRef {
    int first = -1;
    int rest  = -1;
    float gapPx = 0.0f;
};

inline FirstGlyphTextRef AddFirstGlyphText(
    UIFrame& frame, TextStyleId firstStyle, TextStyleId restStyle,
    std::string textUtf8, float x, float y, float layoutW, float layoutH,
    Render::Color4F firstTint, Render::Color4F restTint, float z = 0.0f,
    float gapPx = 0.0f, Render::Layer layer = Render::Layer::HUD
) {
    FirstGlyphTextRef ref;
    ref.gapPx = gapPx;

    std::string firstCp, restCp;
    Utils::SplitFirstCp(textUtf8, firstCp, restCp);

    if (!firstCp.empty()) {
        ref.first = PushText(frame, firstStyle, std::move(firstCp),
            x, y, layoutW, layoutH, firstTint, z, layer);
    }
    if (!restCp.empty()) {
        ref.rest = PushText(frame, restStyle, std::move(restCp),
            x, y, layoutW, layoutH, restTint, z, layer);
    }
    return ref;
}

inline void PlaceFirstGlyphText(UIFrame& frame, const FirstGlyphTextRef& ref, float x, float y) {
    TextOp* firstOp = GetText(frame, ref.first);
    TextOp* restOp  = GetText(frame, ref.rest);
    if (!firstOp || !restOp) return;

    float maxH = 0.0f;
    if (firstOp) maxH = (std::max)(maxH, static_cast<float>(firstOp->baked.h));
    if (restOp)  maxH = (std::max)(maxH, static_cast<float>(restOp->baked.h));

    float curX = x;
    auto Place = [&](TextOp* op) {
        if (!op) return;
        op->x = curX;
        op->y = y + maxH - static_cast<float>(op->baked.h);
        curX += static_cast<float>(op->baked.w) + ref.gapPx;
    };

    Place(firstOp);
    if (firstOp && restOp) curX += ref.gapPx;
    Place(restOp);
}

inline void CenterFirstGlyphTextInRect(UIFrame& frame, const FirstGlyphTextRef& ref, const Render::RectF& rect) {
    TextOp* firstOp = GetText(frame, ref.first);
    TextOp* restOp  = GetText(frame, ref.rest);
    if (!firstOp || !restOp) return;

    float wFirst = firstOp ? static_cast<float>(firstOp->baked.w) : 0.0f;
    float wRest  = restOp  ? static_cast<float>(restOp->baked.w)  : 0.0f;
    float gap = (firstOp && restOp) ? ref.gapPx : 0.0f;
    float totalW = wFirst + gap + wRest;

    float maxH = 0.0f;
    if (firstOp) maxH = (std::max)(maxH, static_cast<float>(firstOp->baked.h));
    if (restOp)  maxH = (std::max)(maxH, static_cast<float>(restOp->baked.h));

    float startX = rect.x + (rect.w - totalW) * 0.5f;
    float y = rect.y + (rect.h - maxH) * 0.5f;

    if (firstOp) {
        firstOp->x = startX;
        firstOp->y = y + maxH - static_cast<float>(firstOp->baked.h);
        startX += wFirst + gap;
    }
    if (restOp) {
        restOp->x = startX;
        restOp->y = y + maxH - static_cast<float>(restOp->baked.h);
    }
}
    
} // namespace Salt2D::Game::UI

#endif // GAME_UI_FRAMEWORK_UIBUILDER_H
