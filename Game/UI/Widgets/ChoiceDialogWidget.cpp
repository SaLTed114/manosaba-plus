// Game/UI/Widgets/ChoiceDialogWidget.cpp
#include "ChoiceDialogWidget.h"
#include "Utils/MathUtils.h"

namespace Salt2D::Game::UI {

void ChoiceDialogWidget::Build(const ChoiceHudModel& model, uint32_t canvasW, uint32_t canvasH, UIFrame& frame) {
    visible_ = model.visible;
    if (!model.visible) return;

    optionCount_ = static_cast<int>(model.options.size());
    if (optionCount_ <= 0) return;

    selectedOption_ = Utils::ClampWarp(model.selectedOption, optionCount_);
    
    itemButtons_.clear();
    itemButtons_.reserve(optionCount_);

    const float barW = canvasW * cfg_.barWRatio;
    const float totalH = optionCount_ * cfg_.barH + (optionCount_ - 1) * cfg_.barGap;

    const float baseX = (canvasW - barW) * 0.5f;
    float y = (canvasH - totalH) * 0.5f;

    for (int i = 0; i < optionCount_; i++) {
        Render::RectF itemRect = RectXYWH(baseX, y, barW, cfg_.barH);

        UIButtonRef button = PushButton(frame, MakeHitKey(HitKind::ChoiceOption, i),
            itemRect, TextureId::White, cfg_.barTint,
            TextStyleId::ChoiceSmall, model.options[i].second, cfg_.textTint,
            0.1f, 0.2f);
        
        itemButtons_.push_back(button);

        y += cfg_.barH + cfg_.barGap;
    }
}

void ChoiceDialogWidget::AfterBake(UIFrame& frame) {
    if (!visible_) return;

    // center text horizontally and vertically in the bar
    for (int i = 0; i < optionCount_; i++) {
        CenterTextInRect(frame, itemButtons_[i].text, itemButtons_[i].rect);
    }
}

void ChoiceDialogWidget::ApplyHover(UIFrame& frame, HitKey hoveredKey) {
    int hitIndex = -1;
    if (HitKeyKind(hoveredKey) == HitKind::ChoiceOption) {
        hitIndex = static_cast<int>(HitKeyIndex(hoveredKey));
    } else if (kbFallbackEnabled_) {
        hitIndex = selectedOption_;
    }

    for (int i = 0; i < optionCount_; i++) {
        if (SpriteOp* sprite = GetSprite(frame, itemButtons_[i].sprite)) {
            sprite->tint = cfg_.barTint;
        }
        if (TextOp* text = GetText(frame, itemButtons_[i].text)) {
            text->tint = cfg_.textTint;
        }
    }

    if (hitIndex < 0 || hitIndex >= optionCount_) return;

    if (SpriteOp* sprite = GetSprite(frame, itemButtons_[hitIndex].sprite)) {
        sprite->tint = cfg_.barHoverTint;
    }
    if (TextOp* text = GetText(frame, itemButtons_[hitIndex].text)) {
        text->tint = cfg_.textHoverTint;
    }
}

bool ChoiceDialogWidget::TryCommit(HitKey clickedKey, int& outIndex) const {
    if (HitKeyKind(clickedKey) != HitKind::ChoiceOption) return false;
    int hitIndex = static_cast<int>(HitKeyIndex(clickedKey));
    if (hitIndex < 0 || hitIndex >= optionCount_) return false;

    outIndex = hitIndex;
    return true;
}

} // namespace Salt2D::Game::UI
