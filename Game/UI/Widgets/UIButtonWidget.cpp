// Game/UI/Widgets/UIButtonWidget.cpp
#include "UIButtonWidget.h"
#include <iostream>

namespace Salt2D::Game::UI {

void UIButtonWidget::Build(UIFrame& frame, HitKey key, Render::RectF rect, bool enabled, bool visible) {
    key_ = key;
    rect_ = rect;
    content_ = rect;
    enabled_ = enabled;
    visible_ = visible;
    hovered_ = false;

    sprites_.clear();
    texts_.clear();
    hitIdx_ = -1;

    if (!visible_) return;

    hitIdx_ = PushHit(frame, key_, rect_, enabled_, true);
}

void UIButtonWidget::SetPadding(float l, float t, float r, float b) {
    content_ = Inset(rect_, l, t, r, b);
}

int UIButtonWidget::AddSprite(UIFrame& frame, TextureId texId, RectRel dstRel,
    TintSet tints, float z, Render::Layer layer
) {
    if (!visible_) return -1;

    Render::RectF dst = RectIn(content_, dstRel);
    const Render::Color4F tint = enabled_ ? tints.rest : tints.disabled;

    int idx = PushSprite(frame, texId, dst, tint, z, layer);
    sprites_.push_back(SpriteSlot{ idx, tints });
    return idx;
}

int UIButtonWidget::AddText(UIFrame& frame, TextStyleId styleId, std::string textUtf8, RectRel dstRel,
    float alignX, float alignY, float offsetX, float offsetY, TintSet tints, float z, Render::Layer layer
) {
    if (!visible_) return -1;

    Render::RectF dst = RectIn(content_, dstRel);
    const Render::Color4F tint = enabled_ ? tints.rest : tints.disabled;

    int idx = PushTextBox(frame, styleId, std::move(textUtf8), dst,
        alignX, alignY, offsetX, offsetY, tint, z, layer);
    texts_.push_back(TextSlot{ idx, tints });
    return idx;
}

void UIButtonWidget::AfterBake(UIFrame& frame) {
    if (!visible_) return;

    for (auto& text : texts_) {
        ResolveTextPlacement(frame, text.idx);
    }
}

void UIButtonWidget::ApplyHover(UIFrame& frame, HitKey hoveredKey) {
    if (!visible_) return;
    hovered_ = (hoveredKey == key_);
    ApplyVisualState(frame);
}

void UIButtonWidget::ApplyVisualState(UIFrame& frame) {
    const bool useHover = hovered_ && enabled_;
    for (const auto& sprite : sprites_) {
        if (SpriteOp* op = GetSprite(frame, sprite.idx)) {
            op->tint = enabled_ ? (useHover ? sprite.tints.hover : sprite.tints.rest) : sprite.tints.disabled;
        }
    }
    for (const auto& text : texts_) {
        if (TextOp* op = GetText(frame, text.idx)) {
            op->tint = enabled_ ? (useHover ? text.tints.hover : text.tints.rest) : text.tints.disabled;
        }
    }

    if (hitIdx_ < 0 || static_cast<size_t>(hitIdx_) >= frame.hits.size()) return;
    HitOp& hit = frame.hits[hitIdx_];
    hit.enabled = enabled_;
    hit.visible = visible_;
}

bool UIButtonWidget::TryClick(HitKey clickedKey) const {
    if (!visible_ || !enabled_) return false;
    return (clickedKey == key_);
}

} // namespace Salt2D::Game::UI
