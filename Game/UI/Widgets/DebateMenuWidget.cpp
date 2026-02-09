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
    
    const float barW = w * cfg_.barWRatio;
    const float totalH = (optionCount_ + 1) * cfg_.barH + (optionCount_) * cfg_.barGap; // with back button

    const float baseX = (w - barW) * 0.5f;
    float y = (h - totalH) * 0.5f;

    optionBtns_.clear();
    optionBtns_.reserve(optionCount_);
    for (int i = 0; i < optionCount_; i++) {
        Render::RectF itemRect = RectXYWH(baseX, y, barW, cfg_.barH);

        UIButtonWidget btn;
        btn.Build(frame, MakeHitKey(HitKind::DebateOption, i), itemRect, true, true);
        btn.AddSprite(frame, TextureId::White, RectRel{0,0,1,1}, TintSet{cfg_.barTint, cfg_.barHoverTint}, 0.35f);
        btn.AddText(frame, TextStyleId::ChoiceSmall, model.menuOptions[i].second, RectRel{0,0,1,1},
            0.5f, 0.5f, 0.0f, 0.0f, TintSet{cfg_.textTint, cfg_.textHoverTint}, 0.4f);
        
        optionBtns_.push_back(std::move(btn));

        y += cfg_.barH + cfg_.barGap;
    }

    Render::RectF backRect = RectXYWH(baseX, y, barW, cfg_.barH);
    backBtn_.Build(frame, MakeHitKey(HitKind::DebateBack, 0), backRect, true, true);
    backBtn_.AddSprite(frame, TextureId::White, RectRel{0,0,1,1}, TintSet{cfg_.barTint, cfg_.barHoverTint}, 0.35f);
    backBtn_.AddText(frame, TextStyleId::ChoiceSmall, "Back", RectRel{0,0,1,1},
        0.5f, 0.5f, 0.0f, 0.0f, TintSet{cfg_.textTint, cfg_.textHoverTint}, 0.4f);
}

void DebateMenuWidget::AfterBake(UIFrame& frame) {
    if (!visible_) return;

    for (auto& btn : optionBtns_) btn.AfterBake(frame);
    backBtn_.AfterBake(frame);
}

void DebateMenuWidget::ApplyHover(UIFrame& frame, HitKey hoveredKey) {
    if (!visible_) return;
    
    for (auto& btn : optionBtns_) btn.ApplyHover(frame, hoveredKey);
    backBtn_.ApplyHover(frame, hoveredKey);
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
