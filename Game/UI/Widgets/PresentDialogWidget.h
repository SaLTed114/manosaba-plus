// Game/UI/Widgets/PresentDialogWidget.h
#ifndef GAME_UI_WIDGETS_PRESENT_DIALOG_WIDGET_H
#define GAME_UI_WIDGETS_PRESENT_DIALOG_WIDGET_H

#include "Game/UI/Framework/UIFrame.h"
#include "Game/UI/Framework/UIBuilder.h"
#include "Game/UI/UITypes.h"
#include <vector>
#include <string>

namespace Salt2D::Game::UI {

struct PresentHudConfig {
    Render::Color4F panelTint    {0.4f,0.4f,0.4f,0.80f};
    Render::Color4F slotTint     {1,1,1,0.40f};
    Render::Color4F slotHoverTint{1,1,1,0.70f};
    Render::Color4F slotSelTint  {1,0.4f,0.4f,0.85f};

    Render::Color4F imgTint      {1,1,1,0.60f};

    Render::Color4F showTint     {1,1,1,0.60f};
    Render::Color4F showHoverTint{1,0.4f,0.4f,1.00f};

    Render::Color4F textTint     {1,1,1,1};

    RectScale imgScale      {2.25f/16.0f, 1.875f/9.0f, 3.2f/16.0f, 3.2f/9.0f};
    RectScale showBtnScale  {2.25f/16.0f, 5.5f/9.0f, 3.2f/16.0f, 1.0f/9.0f};
    RectScale titleScale    {9.8f/16.0f, 0.8f/9.0f, 0, 0};
    RectScale detailScale   {9.5f/16.0f, 2.4f/9.0f, 0, 0};
    RectScale slotListScale {0.5f/16.0f, 7.2f/9.0f, 0, 0};
    RectScale slotScale     {0, 0, 1.4f/16.0f, 1.0f/9.0f};
    RectScale slotGapScale  {0.1f/16.0f, 0, 0, 0};
    RectScale slotInsetScale{0.25f/16.0f, 0.05f/9.0f, 0, 0};
};

class PresentDialogWidget {
public:
    void SetHudConfig(PresentHudConfig cfg) { cfg_ = cfg; }
    void Build(const PresentHudModel& model, uint32_t canvasW, uint32_t canvasH, UIFrame& frame);
    void AfterBake(UIFrame& frame);

    void ApplyHover(UIFrame& frame, HitKey hoveredKey);

    bool Visible() const { return visible_; }
    void SetVisible(bool v) { visible_ = v; }

private:
    PresentHudConfig cfg_{};
    bool visible_ = false;

    int itemCount_ = 0;
    int selectedItem_ = 0;

    std::vector<int> idxSlotSprite_;

    UIButtonRef showButton_;
};

} // namespace Salt2D::Game::UI

#endif // GAME_UI_WIDGETS_PRESENT_DIALOG_WIDGET_H
