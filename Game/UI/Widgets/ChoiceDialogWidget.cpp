// Game/UI/Widgets/ChoiceDialogWidget.cpp
#include "ChoiceDialogWidget.h"
#include "Utils/MathUtils.h"

namespace Salt2D::Game::UI {

void ChoiceDialogWidget::Build(const ChoiceHudModel& model, uint32_t canvasW, uint32_t canvasH, UIFrame& frame) {
    visible_ = model.visible;
    if (!model.visible) return;

    optionCount_ = static_cast<int>(model.options.size());
    if (optionCount_ <= 0) return;

    const float barW = canvasW * cfg_.barWRatio;
    const float totalH = optionCount_ * cfg_.barH + (optionCount_ - 1) * cfg_.barGap;

    const float baseX = (canvasW - barW) * 0.5f;
    float y = (canvasH - totalH) * 0.5f;

    itemBtns_.clear();
    itemBtns_.reserve(optionCount_);
    for (int i = 0; i < optionCount_; i++) {
        Render::RectF itemRect = RectXYWH(baseX, y, barW, cfg_.barH);

        UIButtonWidget btn;
        btn.Build(frame, MakeHitKey(HitKind::ChoiceOption, i), itemRect, true, true);
        btn.AddSprite(frame, TextureId::White, RectRel{0,0,1,1}, TintSet{cfg_.barTint, cfg_.barHoverTint}, 0.1f);
        btn.AddText(frame, TextStyleId::ChoiceSmall, model.options[i].second, RectRel{0,0,1,1},
            0.5f, 0.5f, 0.0f, 0.0f, TintSet{cfg_.textTint, cfg_.textHoverTint}, 0.2f);

        itemBtns_.push_back(std::move(btn));

        y += cfg_.barH + cfg_.barGap;
    }
}

void ChoiceDialogWidget::AfterBake(UIFrame& frame) {
    if (!visible_) return;

    for (auto& btn : itemBtns_) btn.AfterBake(frame); 
}

void ChoiceDialogWidget::ApplyHover(UIFrame& frame, HitKey hoveredKey) {
    for (auto& btn : itemBtns_) btn.ApplyHover(frame, hoveredKey);
}

bool ChoiceDialogWidget::TryCommit(HitKey clickedKey, int& outIndex) const {
    if (HitKeyKind(clickedKey) != HitKind::ChoiceOption) return false;
    int hitIndex = static_cast<int>(HitKeyIndex(clickedKey));
    if (hitIndex < 0 || hitIndex >= optionCount_) return false;

    outIndex = hitIndex;
    return true;
}

} // namespace Salt2D::Game::UI
