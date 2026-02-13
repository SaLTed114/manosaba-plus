// Game/UI/Widgets/VnAutoWidget.cpp
#include "VnAutoWidget.h"
#include "Utils/StringUtils.h"
#include "Utils/MathUtils.h"

namespace Salt2D::Game::UI {

void VnAutoWidget::Build(const VnHudModel& model, uint32_t canvasW, uint32_t canvasH, UIFrame& frame) {
    visible_ = model.visible;
    if (!visible_) return;

    Render::RectF rect = RectFromScale(cfg_.rectScale, canvasW, canvasH);

    btn_.Build(frame, MakeHitKey(HitKind::VNAuto, 0), rect, true, true);

    Render::Color4F tint = model.autoMode ? cfg_.bgOnColor : cfg_.bgOffColor;
    btn_.AddSprite(frame, TextureId::White, RectRel{0,0,1,1}, TintSet{tint, cfg_.bgHoverColor}, cfg_.zBg);
    
    std::string text = model.autoMode ? "Auto: ON" : "Auto: OFF";
    btn_.AddText(frame, TextStyleId::VnAuto, std::move(text), RectRel{0,0,1,1},
        0.5f, 0.5f, 0.0f, 0.0f, TintSet{cfg_.textColor, cfg_.textColor}, cfg_.zText);   
}

void VnAutoWidget::AfterBake(UIFrame& frame) {
    if (!visible_) return;

    btn_.AfterBake(frame);
}

void VnAutoWidget::ApplyHover(UIFrame& frame, HitKey hoveredKey) {
    if (!visible_) return;

    btn_.ApplyHover(frame, hoveredKey);
}

bool VnAutoWidget::TryToggle(HitKey clickedKey) const {
    if (!visible_) return false;

    return btn_.TryClick(clickedKey);
}

} // namespace Salt2D::Game::UI
