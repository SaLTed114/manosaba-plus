// Game/UI/Widgets/VnDialogWidget.h
#ifndef GAME_UI_WIDGETS_VNDIALOGWIDGET_H
#define GAME_UI_WIDGETS_VNDIALOGWIDGET_H

#include "Game/UI/Framework/UIFrame.h"

namespace Salt2D::Game::UI {

class VnDialogWidget {
public:
    void SetHudConfig(VnHudConfig cfg) { cfg_ = cfg; }
    void Build(const VnHudModel& model, uint32_t canvasW, uint32_t canvasH, UIFrame& frame);

    bool Visible() const { return visible_; }
    void SetVisible(bool v) { visible_ = v; }

private:
    VnHudConfig cfg_{};
    bool visible_ = false;
};

} // namespace Salt2D::Game::UI

#endif // GAME_UI_WIDGETS_VNDIALOGWIDGET_H
