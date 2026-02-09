// Game/UI/Widgets/TimerWidget.cpp
#include "TimerWidget.h"
#include "Utils/StringUtils.h"
#include "Utils/MathUtils.h"

namespace Salt2D::Game::UI {

void TimerWidget::Build(const Story::StoryView::TimerView& view, uint32_t canvasW, uint32_t canvasH, UIFrame& frame) {
    visible_ = view.active;
    if (!visible_) return;

    rect_ = RectFromScale(cfg_.rectScale, canvasW, canvasH);

    // background
    PushSprite(frame, TextureId::White, rect_, cfg_.bgColor, cfg_.zBg);

    // time text
    std::string text = Utils::FormatMMSS(view.remainSec);
    textIdx_ = PushTextCentered(frame, TextStyleId::Timer, std::move(text), rect_, cfg_.textColor, cfg_.zText);
}

void TimerWidget::AfterBake(UIFrame& frame) {
    if (!visible_) return;

    CenterTextInRect(frame, textIdx_, rect_);
}

} // namespace Salt2D::Game::UI
