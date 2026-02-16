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
    panelRect_ = RectFromScale(cfg_.panelRect, canvasW, canvasH);
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

    const float contentMargin = cfg_.contentMarginScale * panelRect_.w;
    contentRect_ = Render::RectF{
        panelRect_.x + contentMargin,
        panelRect_.y + my + headerH,
        panelRect_.w - 2.0f * contentMargin,
        panelRect_.h - 2.0f * my - headerH
    };

    const float x0 = contentRect_.x + cfg_.indentScale * panelRect_.w;
    const float y0 = contentRect_.y;
    const float lw = contentRect_.w - cfg_.indentScale * panelRect_.w;
    const float lh = 4096.0f; // large enough

    for (const auto& entry : model.entries) {
        std::string speaker = entry.speakerUtf8;
        std::string text = entry.textUtf8;

        RowIds ids;
        ids.speaker = PushTextInRect(frame, TextStyleId::VnNameRest,
            speaker, Render::RectF{x0, y0 + contentH_, lw, lh}, cfg_.textTint, 0.95f);
        ids.body = PushTextInRect(frame, TextStyleId::VnBody,
            text, Render::RectF{x0, y0 + contentH_ + cfg_.speakerGapScale * panelRect_.h, lw, lh}, cfg_.textTint, 0.95f);

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

        const float indent = cfg_.indentScale * panelRect_.w;
        const float speakerGap = cfg_.speakerGapScale * panelRect_.h;
        const float rowGap = cfg_.rowGapScale * panelRect_.h;

        speakerOp->x = contentRect_.x;
        speakerOp->y = contentRect_.y - scrollY_ + yLocal;
        speakerOp->clipEnabled = true;
        speakerOp->clipRect = Render::RectI{
            static_cast<int>(contentRect_.x),
            static_cast<int>(contentRect_.y),
            static_cast<int>(contentRect_.x + contentRect_.w),
            static_cast<int>(contentRect_.y + contentRect_.h)
        };
        yLocal += static_cast<float>(speakerOp->baked.h) + speakerGap;

        bodyOp->x = contentRect_.x + indent;
        bodyOp->y = contentRect_.y - scrollY_ + yLocal;
        bodyOp->clipEnabled = true;
        bodyOp->clipRect = Render::RectI{
            static_cast<int>(contentRect_.x),
            static_cast<int>(contentRect_.y),
            static_cast<int>(contentRect_.x + contentRect_.w),
            static_cast<int>(contentRect_.y + contentRect_.h)
        };
        yLocal += static_cast<float>(bodyOp->baked.h) + rowGap;
    }

    contentH_ = yLocal - cfg_.rowGapScale * panelRect_.h;
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
