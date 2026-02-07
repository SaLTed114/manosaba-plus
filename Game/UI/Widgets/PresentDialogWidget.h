// Game/UI/Widgets/PresentDialogWidget.h
#ifndef GAME_UI_WIDGETS_PRESENT_DIALOG_WIDGET_H
#define GAME_UI_WIDGETS_PRESENT_DIALOG_WIDGET_H

#include "Game/UI/Framework/UIFrame.h"
#include "Game/UI/UITypes.h"
#include <vector>
#include <string>

namespace Salt2D::Game::UI {


struct PresentHudConfig {
    float bookMargin = 0.0f;
    float listH      = 140.0f;
    float listPad    = 18.0f;
    float slotW      = 120.0f;
    float slotH      = 120.0f;
    float slotGap    = 16.0f;

    float detailPad  = 28.0f;
    float imageW     = 320.0f;
    float imageH     = 320.0f;

    float showBtnW   = 200.0f;
    float showBtnH   = 64.0f;
    float showBtnGap = 24.0f;

    Render::Color4F panelTint    {1,1,1,0.25f};
    Render::Color4F slotTint     {1,1,1,0.40f};
    Render::Color4F slotHoverTint{1,1,1,0.70f};
    Render::Color4F slotSelTint  {1,0.4f,0.4f,0.85f};

    Render::Color4F imgTint      {1,1,1,0.60f};

    Render::Color4F showTint     {1,0.4f,0.4f,0.80f};
    Render::Color4F showHoverTint{1,0.4f,0.4f,1.00f};

    Render::Color4F textTint     {1,1,1,1};
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
    std::vector<int> idxSlotThumb_;
    std::vector<int> idxSlotLabel_;

    int idxShowSprite_ = -1;
    int idxShowText_   = -1;

    Render::RectF showRect_{};
};

} // namespace Salt2D::Game::UI

#endif // GAME_UI_WIDGETS_PRESENT_DIALOG_WIDGET_H
