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

    // initial placement, will adjust after bake
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
    Render::Layer layer = Render::Layer::HUD,
    Transform2D transform = {}
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

    // initial placement, will adjust after bake
    op.x = rect.x;
    op.y = rect.y;

    op.tint = tint;
    op.z = z;
    
    op.transform = transform;

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

inline Render::RectF ComputeTransformedAABB(float x, float y, float w, float h, const Transform2D& t) {
    if (w <= 0.0f || h <= 0.0f) return Render::RectF{x,y,0,0};
    if (!t.hasTransform) return Render::RectF{x,y,w,h};

    // Compute the AABB of the transformed rectangle
    const float px = t.pivotX * w;
    const float py = t.pivotY * h;
    const float sx = t.scaleX;
    const float sy = t.scaleY;

    const float cosR = std::cos(t.rotRad);
    const float sinR = std::sin(t.rotRad);

    auto Transform = [&](float localX, float localY, float& outX, float& outY) {
        // Apply pivot and scale
        float tx = (localX - px) * sx;
        float ty = (localY - py) * sy;

        // Apply rotation
        float rotX = tx * cosR - ty * sinR;
        float rotY = tx * sinR + ty * cosR;

        // Apply translation
        outX = x + rotX + px;
        outY = y + rotY + py;
    };

    float x00, y00, x10, y10, x01, y01, x11, y11;
    Transform(0, 0, x00, y00);
    Transform(w, 0, x10, y10);
    Transform(0, h, x01, y01);
    Transform(w, h, x11, y11);

    float minX = (std::min)({x00, x10, x01, x11});
    float maxX = (std::max)({x00, x10, x01, x11});
    float minY = (std::min)({y00, y10, y01, y11});
    float maxY = (std::max)({y00, y10, y01, y11});

    return Render::RectF{minX, minY, maxX - minX, maxY - minY};
}

inline void UpdateTextAABB(TextOp& op) {
    const float bw = static_cast<float>(op.baked.w);
    const float bh = static_cast<float>(op.baked.h);
    op.aabb = ComputeTransformedAABB(op.x, op.y, bw, bh, op.transform);
}

inline void SetHitRectFromTextAABB(UIFrame& frame, int hitIdx, int textIdx) {
    HitOp* hitOp = GetHit(frame, hitIdx);
    TextOp* textOp = GetText(frame, textIdx);
    if (!hitOp || !textOp) return;

    const float bw = static_cast<float>(textOp->baked.w);
    const float bh = static_cast<float>(textOp->baked.h);

    hitOp->rect = textOp->aabb;

    hitOp->baseRect = Render::RectF{textOp->x, textOp->y, bw, bh};
    hitOp->transform = textOp->transform;
    hitOp->hasTransform = textOp->transform.hasTransform;
}

// =============================
// Transform helper
// =============================

inline Transform2D TransformRotateRad(float rotRad, float pivotX = 0.5f, float pivotY = 0.5f) {
    Transform2D t{};
    t.hasTransform = true;
    t.rotRad = rotRad;
    t.pivotX = pivotX; t.pivotY = pivotY;
    t.scaleX = 1.0f;   t.scaleY = 1.0f;
    return t;
}
    
} // namespace Salt2D::Game::UI

#endif // GAME_UI_FRAMEWORK_UIBUILDER_H
