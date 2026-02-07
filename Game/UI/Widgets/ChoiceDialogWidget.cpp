// Game/UI/Widgets/ChoiceDialogWidget.cpp
#include "ChoiceDialogWidget.h"

namespace Salt2D::Game::UI {

static int ClampWarp(int v, int n) {
    if (n <= 0) return 0;
    v %= n;
    if (v < 0) v += n;
    return v;
}

void ChoiceDialogWidget::Build(const ChoiceHudModel& model, uint32_t canvasW, uint32_t canvasH, UIFrame& frame) {
    visible_ = model.visible;
    if (!model.visible) return;
    optionCount_ = static_cast<int>(model.options.size());
    if (optionCount_ <= 0) return;

    selectedOption_ = ClampWarp(model.selectedOption, optionCount_);
    
    itemRects_.clear();
    itemRects_.reserve(optionCount_);

    idxItemSprite_.clear();
    idxItemSprite_.reserve(optionCount_);
    
    idxItemText_.clear();
    idxItemText_.reserve(optionCount_);

    const float barW = canvasW * cfg_.barWRatio;
    const float totalH = optionCount_ * cfg_.barH + (optionCount_ - 1) * cfg_.barGap;

    const float baseX = (canvasW - barW) * 0.5f;
    float y = (canvasH - totalH) * 0.5f;

    for (int i = 0; i < optionCount_; i++) {
        Render::RectF itemRect{
            baseX, y,
            barW, cfg_.barH};

        itemRects_.push_back(itemRect);

        {
            SpriteOp op;
            op.layer = Render::Layer::HUD;
            op.texId = TextureId::Checker;
            op.dst = itemRect;
            op.tint = cfg_.barTint;
            op.z = 0.1f;

            idxItemSprite_.push_back(static_cast<int>(frame.sprites.size()));
            frame.sprites.push_back(std::move(op));
        }

        {
            TextOp op;
            op.layer = Render::Layer::HUD;
            op.styleId = TextStyleId::ChoiceSmall;
            op.textUtf8 = model.options[i].second;

            op.layoutW = itemRect.w;
            op.layoutH = itemRect.h;

            op.x = itemRect.x;
            op.y = itemRect.y;
            op.tint = cfg_.textTint;
            op.z = 0.2f;

            idxItemText_.push_back(static_cast<int>(frame.texts.size()));
            frame.texts.push_back(std::move(op));
        }

        {
            HitOp op;
            op.key = MakeHitKey(HitKind::ChoiceOption, i);
            op.rect = itemRect;
            op.enabled = true;
            op.visible = true;
            frame.hits.push_back(std::move(op));
        }

        y += cfg_.barH + cfg_.barGap;
    }
}

void ChoiceDialogWidget::AfterBake(UIFrame& frame) {
    if (!visible_) return;

    // center text horizontally and vertically in the bar
    for (int i = 0; i < optionCount_; i++) {
        const int textIdx = idxItemText_[i];
        if (textIdx < 0 || textIdx >= static_cast<int>(frame.texts.size())) continue;

        auto& textOp = frame.texts[textIdx];
        const auto& itemRect = itemRects_[i];

        const float textW = static_cast<float>(textOp.baked.w);
        const float offsetX = (itemRect.w - textW) * 0.5f;
        textOp.x = itemRect.x + offsetX;

        const float textH = static_cast<float>(textOp.baked.h);
        const float offsetY = (itemRect.h - textH) * 0.5f;
        textOp.y = itemRect.y + offsetY;
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
        const int spriteIdx = idxItemSprite_[i];
        const int textIdx = idxItemText_[i];
        
        if (spriteIdx >= 0 && spriteIdx < static_cast<int>(frame.sprites.size())) {
            frame.sprites[spriteIdx].tint = cfg_.barTint;
        }
        if (textIdx >= 0 && textIdx < static_cast<int>(frame.texts.size())) {
            frame.texts[textIdx].tint = cfg_.textTint;
        }
    }

    if (hitIndex < 0 || hitIndex >= optionCount_) return;

    {
        const int spriteIdx = idxItemSprite_[hitIndex];
        const int textIdx = idxItemText_[hitIndex];

        if (spriteIdx >= 0 && spriteIdx < static_cast<int>(frame.sprites.size())) {
            frame.sprites[spriteIdx].tint = cfg_.barHoverTint;
        }
        if (textIdx >= 0 && textIdx < static_cast<int>(frame.texts.size())) {
            frame.texts[textIdx].tint = cfg_.textHoverTint;
        }
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
