// Game/UI/Widgets/VnDialogWidget.h
#ifndef GAME_UI_WIDGETS_VNDIALOGWIDGET_H
#define GAME_UI_WIDGETS_VNDIALOGWIDGET_H

#include "Game/UI/Framework/UIFrame.h"

namespace Salt2D::Game::UI {

class VnDialogWidget {
public:
    void SetHudConfig(VnHudConfig cfg) { cfg_ = cfg; }
    void Build(const VnHudModel& model, uint32_t canvasW, uint32_t canvasH, UIFrame& frame);
    void AfterBake(UIFrame& frame);

    bool Visible() const { return visible_; }
    void SetVisible(bool v) { visible_ = v; }

private:
    VnHudConfig cfg_{};
    bool visible_ = false;

    int idxFam1_      = -1;
    int idxFamRest_   = -1;
    int idxGiven1_    = -1;
    int idxGivenRest_ = -1;

    float nameBaseX_ = 0.0f;
    float nameBaseY_ = 0.0f;
};

} // namespace Salt2D::Game::UI

#endif // GAME_UI_WIDGETS_VNDIALOGWIDGET_H
