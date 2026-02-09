// Game/UI/Widgets/DebateSpeedWidget.h
#ifndef GAME_UI_WIDGETS_DEBATESPEEDWIDGET_H
#define GAME_UI_WIDGETS_DEBATESPEEDWIDGET_H

#include "Game/UI/Framework/UIFrame.h"
#include "Game/UI/Framework/UIBuilder.h"
#include "UIButtonWidget.h"
#include "Game/UI/UITypes.h"

namespace Salt2D::Game::UI {

struct DebateSpeedConfig {
    RectScale rect{11.8f / 16.0f, 0.45f / 9.0f, 0.75f / 16.0f, 0.75f / 9.0f};

    Render::Color4F textTint{1.0f, 1.0f, 1.0f, 1.0f};
    Render::Color4F textHoverTint{0.0f, 0.0f, 0.0f, 1.0f};
    Render::Color4F bgTint{0.0f, 0.0f, 0.0f, 0.5f};
    Render::Color4F bgHoverTint{1.0f, 0.4f, 0.4f, 0.85f};
};

class DebateSpeedWidget {
public:
    void SetConfig(DebateSpeedConfig cfg) { cfg_ = cfg; }
    void Build(DebateHudModel& model, uint32_t canvasW, uint32_t canvasH, UIFrame& frame);
    void AfterBake(UIFrame& frame);

    void ApplyHover(UIFrame& frame, HitKey hoveredKey);
    bool TryHold(HitKey clickedKey) const;

    bool Visible() const { return visible_; }
    void SetVisible(bool v) { visible_ = v; }

private:
    DebateSpeedConfig cfg_;
    bool visible_ = false;

    UIButtonWidget btn_;
};

} // namespace Salt2D::Game::UI

#endif // GAME_UI_WIDGETS_DEBATESPEEDWIDGET_H
