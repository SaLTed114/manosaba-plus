// Game/UI/Widgets/VnAutoWidget.h
#ifndef GAME_UI_WIDGETS_VNAUTOWIDGET_H
#define GAME_UI_WIDGETS_VNAUTOWIDGET_H

#include "Game/Story/StoryView.h"
#include "Game/UI/Framework/UIFrame.h"
#include "Game/UI/Framework/UIBuilder.h"
#include "UIButtonWidget.h"
#include "Game/UI/UITypes.h"

namespace Salt2D::Game::UI {

struct VnAutoConfig {
    RectScale rectScale{0.4f / 16.0f, 8.25f / 9.0f, 1.35f / 16.0f, 0.6f / 9.0f};
    Render::Color4F bgOnColor{1.0f, 1.0f, 0.4f, 0.6f};
    Render::Color4F bgOffColor{1.0f, 1.0f, 1.0f, 0.6f};
    Render::Color4F bgHoverColor{1.0f, 0.4f, 0.4f, 0.8f};
    Render::Color4F textColor{1.0f, 1.0f, 1.0f, 1.0f};
    float zBg = 0.30f;
    float zText = 0.35f;
};

class VnAutoWidget {
public:
    void SetConfig(const VnAutoConfig& cfg) { cfg_ = cfg; }
    void Build(const VnHudModel& model, uint32_t canvasW, uint32_t canvasH, UIFrame& frame);
    void AfterBake(UIFrame& frame);

    void ApplyHover(UIFrame& frame, HitKey hoveredKey);
    bool TryToggle(HitKey clickedKey) const;

    bool Visible() const { return visible_; }
    void SetVisible(bool v) { visible_ = v; }

private:
    VnAutoConfig cfg_{};
    bool visible_ = false;

    UIButtonWidget btn_;
};

} // namespace Salt2D::Game::UI

#endif // GAME_UI_WIDGETS_VNAUTOWIDGET_H
