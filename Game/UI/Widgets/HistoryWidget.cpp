// Game/UI/Widgets/HistoryWidget.cpp
#include "HistoryWidget.h"

namespace Salt2D::Game::UI {

void HistoryWidget::Build(const HistoryModel& model, uint32_t canvasW, uint32_t canvasH, UIFrame& frame) {
    visible_ = model.active;
    rows_.clear();
    if (!visible_) return;

    contentH_ = 0.0f;
    scrollY_ = model.scrollY;

    // background panel
    panelRect_ = RectFromScale(cfg_.rectScale, canvasW, canvasH);
    PushSprite(frame, TextureId::White, panelRect_, cfg_.panelTint, 0.9f);

    // close button
    Render::RectF btnRect = RectFromScale(cfg_.closeBtnRect, canvasW, canvasH);
    closeBtn_.Build(frame, MakeHitKey(HitKind::HistoryClose, 0), btnRect, true, true);
    closeBtn_.AddText(frame, TextStyleId::Timer, "❎", RectRel{0,0,1,1},
        0.5f, 0.5f, 0.0f, 0.0f, TintSet{cfg_.closeBtnTint, cfg_.closeBtnHoverTint}, 0.96f);

    const float mx = cfg_.marginPx;
    const float my = cfg_.marginPx;
    const float headerH = cfg_.headerHPx;

    Render::RectF titleRect{
        panelRect_.x + mx,
        panelRect_.y + my, 
        panelRect_.w - 2.0f * mx,
        headerH
    };
    PushTextInRect(frame, TextStyleId::PresentTitleBig,
        "历史", titleRect, cfg_.textTint, 0.95f);

    contentRect_ = Render::RectF{
        panelRect_.x + mx,
        panelRect_.y + my + headerH,
        panelRect_.w - 2.0f * mx,
        panelRect_.h - 2.0f * my - headerH
    };

    const float x0 = contentRect_.x + cfg_.indentPx;
    const float y0 = contentRect_.y;
    const float lw = contentRect_.w - cfg_.indentPx;
    const float lh = 4096.0f; // large enough

    for (const auto& entry : model.entries) {
        std::string speaker = entry.speakerUtf8;
        std::string text = entry.textUtf8;

        RowIds ids;
        ids.speaker = PushTextInRect(frame, TextStyleId::VnNameRest,
            speaker, Render::RectF{x0, y0 + contentH_, lw, lh}, cfg_.textTint, 0.95f);
        ids.body = PushTextInRect(frame, TextStyleId::VnBody,
            text, Render::RectF{x0, y0 + contentH_ + cfg_.speakerGapPx, lw, lh}, cfg_.textTint, 0.95f);

        rows_.push_back(ids);
    }
}

void HistoryWidget::AfterBake(UIFrame& frame) {
    if (!visible_) return;
    closeBtn_.AfterBake(frame);

    float yLocal = 0.0f;
    for (const auto& ids : rows_) {
        TextOp* speakerOp = GetText(frame, ids.speaker);
        TextOp* bodyOp    = GetText(frame, ids.body);
        if (!speakerOp || !bodyOp) continue;

        speakerOp->x = contentRect_.x + cfg_.indentPx;
        speakerOp->y = contentRect_.y - scrollY_ + yLocal;
        speakerOp->clipEnabled = true;
        speakerOp->clipRect = Render::RectI{
            static_cast<int>(contentRect_.x),
            static_cast<int>(contentRect_.y),
            static_cast<int>(contentRect_.x + contentRect_.w),
            static_cast<int>(contentRect_.y + contentRect_.h)
        };
        yLocal += static_cast<float>(speakerOp->baked.h) + cfg_.speakerGapPx;

        bodyOp->x = contentRect_.x + cfg_.indentPx;
        bodyOp->y = contentRect_.y - scrollY_ + yLocal;
        bodyOp->clipEnabled = true;
        bodyOp->clipRect = Render::RectI{
            static_cast<int>(contentRect_.x),
            static_cast<int>(contentRect_.y),
            static_cast<int>(contentRect_.x + contentRect_.w),
            static_cast<int>(contentRect_.y + contentRect_.h)
        };
        yLocal += static_cast<float>(bodyOp->baked.h) + cfg_.rowGapPx;
    }

    contentH_ = yLocal - cfg_.rowGapPx;
}

void HistoryWidget::ApplyHover(UIFrame& frame, HitKey hoveredKey) {
    if (!visible_) return;
    closeBtn_.ApplyHover(frame, hoveredKey);
}

bool HistoryWidget::TryClose(HitKey clickedKey) const {
    if (!visible_) return false;
    if (closeBtn_.TryClick(clickedKey)) return true;
    return false;
}

} // namespace Salt2D::Game::UI
