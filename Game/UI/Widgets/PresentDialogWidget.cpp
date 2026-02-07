// Game/UI/Widgets/PresentDialogWidget.cpp
#include "PresentDialogWidget.h"
#include "Utils/MathUtils.h"

namespace Salt2D::Game::UI {

void PresentDialogWidget::Build(const PresentHudModel& model, uint32_t canvasW, uint32_t canvasH, UIFrame& frame) {
    visible_ = model.visible;
    if (!model.visible) return;

    itemCount_ = static_cast<int>(model.items.size());
    if (itemCount_ == 0) return;

    selectedItem_ = Utils::ClampWarp(model.selectedItem, itemCount_);

    idxSlotSprite_.clear(); idxSlotSprite_.reserve(itemCount_);

    const float w = static_cast<float>(canvasW);
    const float h = static_cast<float>(canvasH);
    
    // Panel background
    PushSprite(frame, TextureId::White, RectXYWH(0, 0, w, h), cfg_.panelTint, 0.0f);

    // Detail image
    PushSprite(frame, TextureId::Checker, RectFromScale(cfg_.imgScale, canvasW, canvasH), cfg_.imgTint, 0.1f);

    // Detail title/desc
    const std::string& selId    = model.items[selectedItem_].first;
    const std::string& selLabel = model.items[selectedItem_].second;

    titleText_ = AddFirstGlyphText(frame,
        TextStyleId::PresentTitleBig, TextStyleId::PresentTitleRest,
        selLabel, cfg_.titleScale.x * w, cfg_.titleScale.y * h, 800.0f, 60.0f,
        cfg_.textTint, cfg_.textTint, 0.25f, 0.0f);
    titleX_ = cfg_.titleScale.x * w; titleY_ = cfg_.titleScale.y * h;

    PushText(frame, TextStyleId::PresentDetail, std::string("TODO: details for\n") + selId,
        cfg_.detailScale.x * w, cfg_.detailScale.y * h, 800.0f, 400.0f,
        cfg_.textTint, 0.25f);

    // Show button
    showButton_ = PushButton(frame, MakeHitKey(HitKind::PresentShow, 0),
        RectFromScale(cfg_.showBtnScale, canvasW, canvasH),
        TextureId::Checker, cfg_.showTint,
        TextStyleId::VnBody, "", cfg_.textTint,
        0.15f, 0.25f);

    showButtonText_ = AddFirstGlyphText(frame,
        TextStyleId::PresentShowBig, TextStyleId::PresentShowRest,
        "出示", showButton_.rect.x, showButton_.rect.y, showButton_.rect.w, showButton_.rect.h,
        cfg_.showHoverTint, cfg_.textTint, 0.25f, 4.0f);

    // Bottom filmstrip item list
    float x = cfg_.slotListScale.x * w;
    const float y = cfg_.slotListScale.y * h;

    for (int i = 0; i < itemCount_; i++) {
        Render::RectF slotRect = RectXYWH(x, y, cfg_.slotScale.w * w, cfg_.slotScale.h * h);

        // Slot background
        idxSlotSprite_.push_back(PushSprite(frame, TextureId::White, slotRect, cfg_.slotTint, 0.05f));

        // Slot thumbnail
        const float insetW = cfg_.slotInsetScale.x * w;
        const float insetH = cfg_.slotInsetScale.y * h;
        Render::RectF thumbRect = RectXYWH(
            slotRect.x + insetW, slotRect.y + insetH,
            slotRect.w - insetW * 2.0f, slotRect.h - insetH * 2.0f);
        PushSprite(frame, TextureId::Checker, thumbRect, cfg_.imgTint, 0.1f);

        // Slot hit
        PushHit(frame, MakeHitKey(HitKind::PresentItem, i), slotRect);

        x += cfg_.slotScale.w * w + cfg_.slotGapScale.x * w;
    }
}

void PresentDialogWidget::AfterBake(UIFrame& frame) {
    if (!visible_) return;

    // center show button text
    CenterFirstGlyphTextInRect(frame, showButtonText_, showButton_.rect);
    PlaceFirstGlyphText(frame, titleText_, titleX_, titleY_);
}

void PresentDialogWidget::ApplyHover(UIFrame& frame, HitKey hoveredKey) {
    if (!visible_) return;

    int hoveredIndex = -1;
    if (HitKeyKind(hoveredKey) == HitKind::PresentItem) {
        hoveredIndex = static_cast<int>(HitKeyIndex(hoveredKey));
        if (hoveredIndex < 0 || hoveredIndex >= itemCount_) hoveredIndex = -1;
    }

    if (selectedItem_ >= 0 && selectedItem_ < itemCount_) {
        if (SpriteOp* sprite = GetSprite(frame, idxSlotSprite_[selectedItem_])) {
            sprite->tint = cfg_.slotSelTint;
        }
    }

    if (hoveredIndex >= 0 && hoveredIndex < itemCount_ && hoveredIndex != selectedItem_) {
        if (SpriteOp* sprite = GetSprite(frame, idxSlotSprite_[hoveredIndex])) {
            sprite->tint = cfg_.slotHoverTint;
        }
    }

    const bool hoverShow = (HitKeyKind(hoveredKey) == HitKind::PresentShow);
    if (SpriteOp* sprite = GetSprite(frame, showButton_.sprite)) {
        sprite->tint = hoverShow ? cfg_.showHoverTint : cfg_.showTint;
    }
}

} // namespace Salt2D::Game::UI
