// Game/UI/Widgets/ChoiceDialogWidget.h
#ifndef GAME_UI_WIDGETS_CHOICE_DIALOG_WIDGET_H
#define GAME_UI_WIDGETS_CHOICE_DIALOG_WIDGET_H

#include "Game/UI/Framework/UIFrame.h"
#include "Game/UI/Framework/UIBuilder.h"
#include "UIButtonWidget.h"
#include <vector>
#include <string>

namespace Salt2D::Game::UI {

struct ChoiceHudConfig {
    float barWRatio = 0.60f;
    float barH      = 90.0f;
    float barGap    = 16.0f;

    Render::Color4F barTint{1.0f, 1.0f, 1.0f, 0.50f};
    Render::Color4F barHoverTint{1.0f, 0.4f, 0.4f, 0.85f};
    Render::Color4F textTint{1.0f, 1.0f, 1.0f, 1.0f};
    Render::Color4F textHoverTint{1.0f, 1.0f, 1.0f, 1.0f};
};

class ChoiceDialogWidget {
public:
    void SetHudConfig(ChoiceHudConfig cfg) { cfg_ = cfg; }
    void Build(const ChoiceHudModel& model, uint32_t canvasW, uint32_t canvasH, UIFrame& frame);
    void AfterBake(UIFrame& frame);

    void ApplyHover(UIFrame& frame, HitKey hoveredKey);
    bool TryCommit(HitKey clickedKey, int& outIndex) const;

    bool Visible() const { return visible_; }
    void SetVisible(bool v) { visible_ = v; }

private:
    ChoiceHudConfig cfg_{};
    bool visible_ = false;

    int optionCount_ = 0;
    std::vector<UIButtonWidget> itemBtns_;
};

} // namespace Salt2D::Game::UI

#endif // GAME_UI_WIDGETS_CHOICE_DIALOG_WIDGET_H
