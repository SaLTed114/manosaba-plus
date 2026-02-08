// Game/UI/Widgets/DebateMenuWidget.cpp
#include "DebateMenuWidget.h"
#include "Utils/MathUtils.h"

namespace Salt2D::Game::UI {

void DebateMenuWidget::Build(const DebateHudModel& model, uint32_t canvasW, uint32_t canvasH, UIFrame& frame) {
    visible_ = model.menuOpen;
    if (!visible_) return;

    optionCount_ = static_cast<int>(model.menuOptions.size());
    if (optionCount_ <= 0) return;

    const float w = static_cast<float>(canvasW);
    const float h = static_cast<float>(canvasH);

    // Background panel
    Render::RectF bgRect = RectXYWH(0, 0, w, h);
    PushSprite(frame, TextureId::White, bgRect, cfg_.backgroundTint, 0.3f);

    // Buttons
    optionBtns_.clear();
    optionBtns_.reserve(optionCount_);

    const float barW = w * cfg_.barWRatio;
    const float totalH = (optionCount_ + 1) * cfg_.barH + (optionCount_) * cfg_.barGap; // with back button

    const float baseX = (w - barW) * 0.5f;
    float y = (h - totalH) * 0.5f;

    for (int i = 0; i < optionCount_; i++) {
        Render::RectF itemRect = RectXYWH(baseX, y, barW, cfg_.barH);

        UIButtonRef button = PushButton(frame, MakeHitKey(HitKind::DebateOption, i),
            itemRect, TextureId::White, cfg_.barTint,
            TextStyleId::ChoiceSmall, model.menuOptions[i].second, cfg_.textTint,
            0.35f, 0.4f);
        
        optionBtns_.push_back(button);

        y += cfg_.barH + cfg_.barGap;
    }

    Render::RectF backRect = RectXYWH(baseX, y, barW, cfg_.barH);
    backBtn_ = PushButton(frame, MakeHitKey(HitKind::DebateBack, 0),
        backRect, TextureId::White, cfg_.barTint,
        TextStyleId::ChoiceSmall, "Back", cfg_.textTint,
        0.35f, 0.4f);
}

void DebateMenuWidget::AfterBake(UIFrame& frame) {
    if (!visible_) return;

    // center text horizontally and vertically in the bar
    for (int i = 0; i < optionCount_; i++) {
        CenterTextInRect(frame, optionBtns_[i].text, optionBtns_[i].rect);
    }
    CenterTextInRect(frame, backBtn_.text, backBtn_.rect);
}

void DebateMenuWidget::ApplyHover(UIFrame& frame, HitKey hoveredKey) {
    int hitIndex = -1;
    if (HitKeyKind(hoveredKey) == HitKind::DebateOption) {
        hitIndex = static_cast<int>(HitKeyIndex(hoveredKey));
    }

    for (int i = 0; i < optionCount_; i++) {
        if (SpriteOp* sprite = GetSprite(frame, optionBtns_[i].sprite)) {
            sprite->tint = (i == hitIndex) ? cfg_.barHoverTint : cfg_.barTint;
        }
        if (TextOp* text = GetText(frame, optionBtns_[i].text)) {
            text->tint = (i == hitIndex) ? cfg_.textHoverTint : cfg_.textTint;
        }
    }

    // Back button hover
    bool backHover = (HitKeyKind(hoveredKey) == HitKind::DebateBack);
    if (SpriteOp* sprite = GetSprite(frame, backBtn_.sprite)) {
        sprite->tint = backHover ? cfg_.barHoverTint : cfg_.barTint;
    }
    if (TextOp* text = GetText(frame, backBtn_.text)) {
        text->tint = backHover ? cfg_.textHoverTint : cfg_.textTint;
    }
}

bool DebateMenuWidget::TryCommit(HitKey clickedKey, int& outIndex) const {
    if (HitKeyKind(clickedKey) != HitKind::DebateOption) return false;
    int hitIndex = static_cast<int>(HitKeyIndex(clickedKey));
    if (hitIndex < 0 || hitIndex >= optionCount_) return false;

    outIndex = hitIndex;
    return true;
}

bool DebateMenuWidget::TryBack(HitKey clickedKey) const {
    return HitKeyKind(clickedKey) == HitKind::DebateBack;
}

} // namespace Salt2D::Game::UI
