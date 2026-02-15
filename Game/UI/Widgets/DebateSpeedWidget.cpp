// Game/UI/Widgets/DebateSpeedWidget.cpp
#include "DebateSpeedWidget.h"
#include "Utils/MathUtils.h"

namespace Salt2D::Game::UI {

inline std::string SpeedLabel(float timeScale) {
    if (std::abs(timeScale - 3.0f) < 1e-3f) return "x3";
    if (std::abs(timeScale - 2.0f) < 1e-3f) return "x2";
    if (std::abs(timeScale - 1.0f) < 1e-3f) return "x1";
    return "Unknown";
}

void DebateSpeedWidget::Build(DebateHudModel& model, uint32_t canvasW, uint32_t canvasH, UIFrame& frame) {
    visible_ = !model.menuOpen;
    if (!visible_) return;

    Render::RectF rect = RectFromScale(cfg_.rect, canvasW, canvasH);
    btn_.Build(frame, MakeHitKey(HitKind::DebateSpeed, 0), rect, true, true);

    Render::Color4F tint = (model.timeScale > 1.0f) ? cfg_.bgHoverTint : cfg_.bgTint;
    btn_.AddSprite(frame, TextureId::White, RectRel{0,0,1,1}, TintSet{tint, cfg_.bgHoverTint}, 0.35f);
    btn_.AddText(frame, TextStyleId::Timer, SpeedLabel(model.timeScale), RectRel{0,0,1,1},
        0.5f, 0.5f, 0.0f, 0.0f, TintSet{cfg_.textTint, cfg_.textHoverTint}, 0.4f);
}

void DebateSpeedWidget::AfterBake(UIFrame& frame) {
    if (!visible_) return;
    btn_.AfterBake(frame);
}

void DebateSpeedWidget::ApplyHover(UIFrame& frame, HitKey hoveredKey) {
    if (!visible_) return;
    btn_.ApplyHover(frame, hoveredKey);
}

bool DebateSpeedWidget::TryHold(HitKey clickedKey) const {
    if (!visible_) return false;
    if (btn_.TryClick(clickedKey)) return true;
    return false;
}

} // namespace Salt2D::Game::UI
