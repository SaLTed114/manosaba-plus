// Game/UI/Framework/UIInteraction.cpp
#include "UIInteraction.h"
#include <cmath>

namespace Salt2D::Game::UI {

static inline bool Contains(const Render::RectF& r, float x, float y) {
    return x >= r.x && x <= (r.x + r.w) && y >= r.y && y <= (r.y + r.h);
}

static inline bool ContainsTransformedRect(
    const Render::RectF& baseRect,
    const Transform2D& t, float x, float y
) {
    const float w = baseRect.w;
    const float h = baseRect.h;
    if (w <= 0.0f || h <= 0.0f) return false;

    const float sx = t.scaleX;
    const float sy = t.scaleY;
    if (sx == 0.0f || sy == 0.0f) return false;

    float dx = x - baseRect.x;
    float dy = y - baseRect.y;

    const float px = t.pivotX * w;
    const float py = t.pivotY * h;

    dx -= px; dy -= py;

    const float cosR = std::cos(-t.rotRad);
    const float sinR = std::sin(-t.rotRad);
    float localX = (dx * cosR - dy * sinR) / sx;
    float localY = (dx * sinR + dy * cosR) / sy;

    localX += px; localY += py;

    return localX >= 0.0f && localX <= w && localY >= 0.0f && localY <= h;
}

static inline HitKey HitTest(const UIFrame& frame, float x, float y) {
    for (int i = static_cast<int>(frame.hits.size()) - 1; i >= 0; --i) {
        const auto& hit = frame.hits[i];
        if (!hit.enabled || !hit.visible) continue;
        if (!Contains(hit.rect, x, y)) continue;

        if (!hit.hasTransform) return hit.key;
        if (ContainsTransformedRect(hit.baseRect, hit.transform, x, y)) return hit.key;
    }
    return 0;
}

UIInteractionResult UIInteraction::Update(
    const UIFrame& frame,
    const Session::ActionFrame& af,
    UIPointerState& state
) {
    UIInteractionResult result{};
    const float mx = static_cast<float>(af.pointer.x);
    const float my = static_cast<float>(af.pointer.y);

    result.hovered = HitTest(frame, mx, my);

    if (af.pointer.lPressed) {
        state.pressed = result.hovered;
    }

    if (af.pointer.lReleased) {
        if (state.pressed != 0 && state.pressed == result.hovered) {
            result.clicked = state.pressed;
        }
        state.pressed = 0;
    }

    if (af.pointer.lDown) {
        result.down = state.pressed;
    }

    state.hovered = result.hovered;
    return result;
}

} // namespace Salt2D::Game::UI
