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
    
    // Panel background
    Render::RectF panelRect{
        cfg_.bookMargin, cfg_.bookMargin,
        static_cast<float>(canvasW) - cfg_.bookMargin * 2.0f,
        static_cast<float>(canvasH) - cfg_.bookMargin * 2.0f
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

    // Bottom item list
    Render::RectF listRect{
        panelRect.x + cfg_.listPad,
        panelRect.y + panelRect.h - cfg_.listH,
        panelRect.w - cfg_.listPad * 2.0f,
        cfg_.listH
    };

    // Prompt
    {
        TextOp op;
        op.layer = Render::Layer::HUD;
        op.styleId = TextStyleId::VnBody;
        op.textUtf8 = model.promptUtf8;
        op.layoutW = Utils::EnsureFinite(panelRect.w - cfg_.detailPad * 2.0f);
        
        op.layoutH = 80.0f;
        op.x = panelRect.x + cfg_.detailPad;
        op.y = panelRect.y + cfg_.detailPad;
        op.tint = cfg_.textTint;
        op.z = 0.25f;
        frame.texts.push_back(std::move(op));
    }

    // Detail region
    Render::RectF detailRect{
        panelRect.x + cfg_.detailPad,
        panelRect.y + cfg_.detailPad + 80.0f,
        panelRect.w - cfg_.detailPad * 2.0f,
        panelRect.h - cfg_.detailPad - cfg_.listH - 80.0f
    };

    // Detail image
    Render::RectF imageRect{
        detailRect.x,
        detailRect.y,
        cfg_.imageW,
        cfg_.imageH
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

    const float textX = imageRect.x + imageRect.w + cfg_.detailPad;
    const float textW = Utils::EnsureFinite(detailRect.x + detailRect.w - textX);

    {
        TextOp op;
        op.layer = Render::Layer::HUD;
        op.styleId = TextStyleId::VnBody;
        op.textUtf8 = selLabel; // placeholder title
        op.layoutW = textW;
        op.layoutH = 60.0f;
        op.x = textX;
        op.y = imageRect.y;
        op.tint = cfg_.textTint;
        op.z = 0.25f;
        frame.texts.push_back(std::move(op));
    }

    {
        TextOp op;
        op.layer = Render::Layer::HUD;
        op.styleId = TextStyleId::VnBody;
        op.textUtf8 = std::string("TODO: details for") + selId; // placeholder desc
        op.layoutW = textW;
        op.layoutH = Utils::EnsureFinite(
            detailRect.y + detailRect.h - (imageRect.y + imageRect.h) - cfg_.showBtnH - cfg_.showBtnGap);
        op.x = textX;
        op.y = imageRect.y + imageRect.h;
        op.tint = cfg_.textTint;
        op.z = 0.25f;
        frame.texts.push_back(std::move(op));
    }

    // Show button
    showRect_ = Render::RectF{
        cfg_.showBtnW, cfg_.showBtnH,
        detailRect.x + detailRect.w - cfg_.showBtnW,
        detailRect.y + detailRect.h - cfg_.showBtnH
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
        op.styleId = TextStyleId::PresentSmall;
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
    float x = listRect.x;
    const float y = listRect.y + (listRect.h - cfg_.slotH) * 0.5f;

    for (int i = 0; i < itemCount_; i++) {
        Render::RectF slotRect{x, y, cfg_.slotW, cfg_.slotH};

        // Slot background
        {
            SpriteOp op;
            op.layer = Render::Layer::HUD;
            op.texId = TextureId::Checker;
            op.dst = slotRect;
            op.tint = cfg_.slotTint;
            op.z = 0.05f;

            idxSlotSprite_.push_back(static_cast<int>(frame.sprites.size()));
            frame.sprites.push_back(std::move(op));
        }

        // Slot thumbnail
        {
            const float inset = 10.0f;
            Render::RectF thumbRect{
                slotRect.x + inset,
                slotRect.y + inset,
                slotRect.w - inset * 2.0f,
                slotRect.h - inset * 2.0f
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

        x += cfg_.slotW + cfg_.slotGap;
        if (x + cfg_.slotW > listRect.x + listRect.w) {
            // Exceed list rect, stop creating more slots
            break;
        }
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
    // else hoveredIndex = selectedItem_; // if not hovering any item, fallback to selected item for hover effect

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
