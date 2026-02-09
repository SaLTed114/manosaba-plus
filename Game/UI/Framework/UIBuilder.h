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

inline HitOp* GetHit(UIFrame& frame, int idx) {
    if (idx < 0 || static_cast<size_t>(idx) >= frame.hits.size()) return nullptr;
    return &frame.hits[idx];
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

    op.rect = Render::RectF{0,0,0,0};
    op.alignX = 0.0f;
    op.alignY = 0.0f;
    op.offsetX = 0.0f;
    op.offsetY = 0.0f;

    int idx = static_cast<int>(frame.texts.size());
    frame.texts.push_back(std::move(op));
    return idx;
}

inline int PushTextBox(
    UIFrame& frame, TextStyleId styleId, std::string textUtf8,
    const Render::RectF& rect,
    float alignX, float alignY, float offsetX, float offsetY,
    const Render::Color4F& tint, float z = 0.0f,
    Render::Layer layer = Render::Layer::HUD
) {
    TextOp op;
    op.layer = layer;
    op.styleId = styleId;
    op.textUtf8 = std::move(textUtf8);

    op.layoutW = rect.w;
    op.layoutH = rect.h;

    op.rect = rect;
    op.alignX = alignX;
    op.alignY = alignY;
    op.offsetX = offsetX;
    op.offsetY = offsetY;

    op.x = rect.x;
    op.y = rect.y;

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
    // align left-top by default, caller can adjust after bake
    return PushTextBox(frame, styleId, std::move(textUtf8), dst,
        0.0f, 0.0f, 0.0f, 0.0f, tint, z, layer);
}

inline int PushTextCentered(
    UIFrame& frame, TextStyleId styleId, std::string textUtf8,
    const Render::RectF& dst, const Render::Color4F& tint, float z = 0.0f,
    Render::Layer layer = Render::Layer::HUD
) {
    // align center by default, caller can adjust after bake
    return PushTextBox(frame, styleId, std::move(textUtf8), dst,
        0.5f, 0.5f, 0.0f, 0.0f, tint, z, layer);
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

// =============================
// AfterBake helpers
// =============================

inline void ResolveTextPlacement(TextOp& op) {
    const float bw = static_cast<float>(op.baked.w);
    const float bh = static_cast<float>(op.baked.h);

    if (op.rect.w > 0.0f && op.rect.h > 0.0f) {
        op.x = op.rect.x + (op.rect.w - bw) * op.alignX + op.offsetX;
        op.y = op.rect.y + (op.rect.h - bh) * op.alignY + op.offsetY;
    }
    op.aabb = Render::RectF{op.x, op.y, bw, bh};
}

inline void ResolveTextPlacement(UIFrame& frame, int textIdx) {
    TextOp* textOp = GetText(frame, textIdx);
    if (!textOp) return;
    ResolveTextPlacement(*textOp);
}


inline void CenterTextInRect(UIFrame& frame, int textIdx, const Render::RectF& rect) {
    TextOp* textOp = GetText(frame, textIdx);
    if (!textOp) return;

    textOp->rect = rect;
    textOp->alignX = 0.5f;
    textOp->alignY = 0.5f;
    textOp->offsetX = 0.0f;
    textOp->offsetY = 0.0f;
    ResolveTextPlacement(*textOp);
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

// =============================
// Hit AABB helper
// =============================

inline void SetHitRectFromTextAABB(UIFrame& frame, int hitIdx, int textIdx) {
    HitOp* hitOp = GetHit(frame, hitIdx);
    TextOp* textOp = GetText(frame, textIdx);
    if (!hitOp || !textOp) return;

    hitOp->rect.x = textOp->x;
    hitOp->rect.y = textOp->y;
    hitOp->rect.w = static_cast<float>(textOp->baked.w);
    hitOp->rect.h = static_cast<float>(textOp->baked.h);
}
    
} // namespace Salt2D::Game::UI

#endif // GAME_UI_FRAMEWORK_UIBUILDER_H
