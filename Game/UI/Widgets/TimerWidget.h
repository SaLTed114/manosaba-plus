// Game/UI/Widgets/TimerWidget.h
#ifndef GAME_UI_WIDGETS_TIMERWIDGET_H
#define GAME_UI_WIDGETS_TIMERWIDGET_H

#include "Game/Story/StoryView.h"
#include "Game/UI/Framework/UIFrame.h"
#include "Game/UI/Framework/UIBuilder.h"
#include "Game/UI/UITypes.h"

namespace Salt2D::Game::UI {

struct TimerConfig {
    RectScale rectScale{12.8f / 16.0f, 0.45f / 9.0f, 2.5f / 16.0f, 0.75f / 9.0f};
    Render::Color4F bgColor{0.0f, 0.0f, 0.0f, 0.6f};
    Render::Color4F textColor{1.0f, 1.0f, 1.0f, 1.0f};
    float zBg = 0.80f;
    float zText = 0.85f;
};

class TimerWidget {
public:
    void SetConfig(const TimerConfig& cfg) { cfg_ = cfg; }
    void Build(const Story::StoryView::TimerView& view, uint32_t canvasW, uint32_t canvasH, UIFrame& frame);
    void AfterBake(UIFrame& frame);

    bool Visible() const { return visible_; }
    void SetVisible(bool v) { visible_ = v; }

private:
    TimerConfig cfg_{};
    bool visible_ = false;

    Render::RectF rect_{};
    int textIdx_ = -1;
};

} // namespace Salt2D::Game::UI

#endif // GAME_UI_WIDGETS_TIMERWIDGET_H
