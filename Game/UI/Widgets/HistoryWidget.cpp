// Game/UI/Widgets/HistoryWidget.cpp
#include "HistoryWidget.h"

namespace Salt2D::Game::UI {

void HistoryWidget::Build(const HistoryModel& model, uint32_t canvasW, uint32_t canvasH, UIFrame& frame) {
    visible_ = model.active;
    if (!visible_) return;

    // placeholder for future history display implementation

    // background panel
    Render::RectF panelRect = RectFromScale(cfg_.rectScale, canvasW, canvasH);
    PushSprite(frame, TextureId::White, panelRect, cfg_.panelTint, 0.9f);

    // close button
    Render::RectF btnRect = RectFromScale(cfg_.closeBtnRect, canvasW, canvasH);
    closeBtn_.Build(frame, MakeHitKey(HitKind::HistoryClose, 0), btnRect, true, true);
    closeBtn_.AddSprite(frame, TextureId::White, RectRel{0,0,1,1}, TintSet{cfg_.closeBtnTint, cfg_.closeBtnHoverTint}, 0.95f);
    closeBtn_.AddText(frame, TextStyleId::Timer, "❎", RectRel{0,0,1,1},
        0.5f, 0.5f, 0.0f, 0.0f, TintSet{Render::Color4F{1,1,1,1}, Render::Color4F{1,1,1,1}}, 0.96f);
}

void HistoryWidget::AfterBake(UIFrame& frame) {
    if (!visible_) return;
    closeBtn_.AfterBake(frame);
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
