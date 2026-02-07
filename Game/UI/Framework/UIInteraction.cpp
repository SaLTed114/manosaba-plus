// Game/UI/Framework/UIInteraction.cpp
#include "UIInteraction.h"

namespace Salt2D::Game::UI {

static inline bool Contains(const Render::RectF& r, float x, float y) {
    return x >= r.x && x <= (r.x + r.w) && y >= r.y && y <= (r.y + r.h);
}

static inline HitKey HitTest(const UIFrame& frame, float x, float y) {
    for (int i = static_cast<int>(frame.hits.size()) - 1; i >= 0; --i) {
        const auto& hit = frame.hits[i];
        if (!hit.enabled || !hit.visible) continue;
        if (Contains(hit.rect, x, y)) return hit.key;
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

    state.hovered = result.hovered;
    return result;
}

} // namespace Salt2D::Game::UI
