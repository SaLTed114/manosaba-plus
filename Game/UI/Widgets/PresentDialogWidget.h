// Game/UI/Widgets/PresentDialogWidget.h
#ifndef GAME_UI_WIDGETS_PRESENT_DIALOG_WIDGET_H
#define GAME_UI_WIDGETS_PRESENT_DIALOG_WIDGET_H

#include "Game/UI/Framework/UIFrame.h"
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

    float imgXScale = 2.25f / 16.0f;
    float imgYScale = 1.875f / 9.0f;
    float imgWScale = 3.2f / 16.0f;
    float imgHScale = 3.2f / 9.0f;

    float showBtnXScale = 2.25f / 16.0f;
    float showBtnYScale = 5.5f / 9.0f;
    float showBtnHScale = 1.0f / 9.0f;
    float showBtnWScale = 3.2f / 16.0f;

    float titleXScale = 9.8f / 16.0f;
    float titleYScale = 0.8f / 9.0f;

    float detailXScale = 9.5f / 16.0f;
    float detailYScale = 2.4f / 9.0f;

    float slotListXScale = 0.5f / 16.0f;
    float slotListYScale = 7.2f / 9.0f;
    float slotWScale = 1.4f / 16.0f;
    float slotHScale = 1.0f / 9.0f;
    float slotWGapScale = 0.1f / 16.0f;
    float slotInsetWScale = 0.25f / 16.0f;
    float slotInsetHScale = 0.05f / 9.0f;
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
