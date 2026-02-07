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
    idxSlotThumb_. clear(); idxSlotThumb_. reserve(itemCount_);
    idxSlotLabel_. clear(); idxSlotLabel_. reserve(itemCount_);

    const float w = static_cast<float>(canvasW);
    const float h = static_cast<float>(canvasH);
    
    // Panel background
    Render::RectF panelRect{
        0.0f, 0.0f,
        static_cast<float>(canvasW),
        static_cast<float>(canvasH)
    };

    {
        SpriteOp op;
        op.layer = Render::Layer::HUD;
        op.texId = TextureId::White;
        op.dst = panelRect;
        op.tint = cfg_.panelTint;
        op.z = 0.0f;
        frame.sprites.push_back(std::move(op));
    }

    // Detail image
    Render::RectF imageRect{
        cfg_.imgXScale * w, cfg_.imgYScale * h,
        cfg_.imgWScale * w, cfg_.imgHScale * h
    };

    {
        SpriteOp op;
        op.layer = Render::Layer::HUD;
        op.texId = TextureId::Checker;
        op.dst = imageRect;
        op.tint = cfg_.imgTint;
        op.z = 0.1f;
        frame.sprites.push_back(std::move(op));
    }

    // Detail title/desc
    const std::string& selId    = model.items[selectedItem_].first;
    const std::string& selLabel = model.items[selectedItem_].second;

    {
        TextOp op;
        op.layer = Render::Layer::HUD;
        op.styleId = TextStyleId::PresentTitleRest;
        op.textUtf8 = selLabel; // placeholder title
        op.layoutW = 800.0f;
        op.layoutH = 60.0f;
        op.x = cfg_.titleXScale * w;
        op.y = cfg_.titleYScale * h;
        op.tint = cfg_.textTint;
        op.z = 0.25f;
        frame.texts.push_back(std::move(op));
    }

    {
        TextOp op;
        op.layer = Render::Layer::HUD;
        op.styleId = TextStyleId::PresentDetail;
        op.textUtf8 = std::string("TODO: details for\n") + selId; // placeholder desc
        op.layoutW = 800.0f;
        op.layoutH = 400.0f;
        op.x = cfg_.detailXScale * w;
        op.y = cfg_.detailYScale * h;
        op.tint = cfg_.textTint;
        op.z = 0.25f;
        frame.texts.push_back(std::move(op));
    }

    // Show button
    showRect_ = Render::RectF{
        cfg_.showBtnXScale * w, cfg_.showBtnYScale * h,
        cfg_.showBtnWScale * w, cfg_.showBtnHScale * h
    };

    {
        SpriteOp op;
        op.layer = Render::Layer::HUD;
        op.texId = TextureId::Checker;
        op.dst = showRect_;
        op.tint = cfg_.showTint;
        op.z = 0.15f;

        idxShowSprite_ = static_cast<int>(frame.sprites.size());
        frame.sprites.push_back(std::move(op));
    }

    {
        TextOp op;
        op.layer = Render::Layer::HUD;
        op.styleId = TextStyleId::VnBody;
        op.textUtf8 = "出示"; // placeholder
        op.layoutW = showRect_.w;
        op.layoutH = showRect_.h;
        op.x = showRect_.x;
        op.y = showRect_.y;
        op.tint = cfg_.textTint;
        op.z = 0.25f;

        idxShowText_ = static_cast<int>(frame.texts.size());
        frame.texts.push_back(std::move(op));
    }

    {
        HitOp op;
        op.key = MakeHitKey(HitKind::PresentShow, 0);
        op.rect = showRect_;
        op.enabled = true;
        op.visible = true;
        frame.hits.push_back(std::move(op));
    }

    // Bottom filmstrip item list
    float x = cfg_.slotListXScale * w;
    const float y = cfg_.slotListYScale * h;

    for (int i = 0; i < itemCount_; i++) {
        Render::RectF slotRect{x, y, cfg_.slotWScale * w, cfg_.slotHScale * h};

        // Slot background
        {
            SpriteOp op;
            op.layer = Render::Layer::HUD;
            op.texId = TextureId::White;
            op.dst = slotRect;
            op.tint = cfg_.slotTint;
            op.z = 0.05f;

            idxSlotSprite_.push_back(static_cast<int>(frame.sprites.size()));
            frame.sprites.push_back(std::move(op));
        }

        // Slot thumbnail
        {
            Render::RectF thumbRect{
                slotRect.x + cfg_.slotInsetWScale * w,
                slotRect.y + cfg_.slotInsetHScale * h,
                slotRect.w - cfg_.slotInsetWScale * 2.0f * w,
                slotRect.h - cfg_.slotInsetHScale * 2.0f * h
            };

            SpriteOp op;
            op.layer = Render::Layer::HUD;
            op.texId = TextureId::Checker;
            op.dst = thumbRect;
            op.tint = cfg_.imgTint;
            op.z = 0.1f;

            idxSlotThumb_.push_back(static_cast<int>(frame.sprites.size()));
            frame.sprites.push_back(std::move(op));
        }

        // Slot hit
        {
            HitOp op;
            op.key = MakeHitKey(HitKind::PresentItem, i);
            op.rect = slotRect;
            op.enabled = true;
            op.visible = true;
            frame.hits.push_back(std::move(op));
        }

        x += cfg_.slotWScale * w + cfg_.slotWGapScale * w;
    }
}

void PresentDialogWidget::AfterBake(UIFrame& frame) {
    if (!visible_) return;

    // center show button text
    if (idxShowText_ >= 0 && idxShowText_ < static_cast<int>(frame.texts.size())) {
        TextOp& textOp = frame.texts[idxShowText_];
        textOp.x = showRect_.x + (showRect_.w - textOp.baked.w) * 0.5f;
        textOp.y = showRect_.y + (showRect_.h - textOp.baked.h) * 0.5f;
    }
}

void PresentDialogWidget::ApplyHover(UIFrame& frame, HitKey hoveredKey) {
    if (!visible_) return;

    const bool hoverItem = (HitKeyKind(hoveredKey) == HitKind::PresentItem);
    const bool hoverShow = (HitKeyKind(hoveredKey) == HitKind::PresentShow);

    int hoveredIndex = -1;
    if (hoverItem) hoveredIndex = static_cast<int>(HitKeyIndex(hoveredKey));

    // reset slots
    for (int i = 0; i < itemCount_; i++) {
        const int spriteIdx = idxSlotSprite_[i];
        if (spriteIdx >= 0 && spriteIdx < static_cast<int>(frame.sprites.size())) {
            frame.sprites[spriteIdx].tint = cfg_.slotTint;
        }
    }

    if (hoverItem && hoveredIndex >= 0 && hoveredIndex < itemCount_) {
        const int spriteIdx = idxSlotSprite_[hoveredIndex];
        if (spriteIdx >= 0 && spriteIdx < static_cast<int>(frame.sprites.size())) {
            frame.sprites[spriteIdx].tint = cfg_.slotHoverTint;
        }
    }

    // show botton
    if (idxShowSprite_ >= 0 && idxShowSprite_ < static_cast<int>(frame.sprites.size())) {
        frame.sprites[idxShowSprite_].tint = hoverShow ? cfg_.showHoverTint : cfg_.showTint;
    }
}

} // namespace Salt2D::Game::UI
