// Game/UI/Widgets/ChoiceDialogWidget.h
#ifndef GAME_UI_WIDGETS_CHOICE_DIALOG_WIDGET_H
#define GAME_UI_WIDGETS_CHOICE_DIALOG_WIDGET_H

#include "Game/UI/Framework/UIFrame.h"
#include <vector>
#include <string>

namespace Salt2D::Game::UI {

class ChoiceDialogWidget {
public:
    void SetHudConfig(ChoiceHudConfig cfg) { cfg_ = cfg; }
    void Build(const ChoiceHudModel& model, uint32_t canvasW, uint32_t canvasH, UIFrame& frame);
    void AfterBake(UIFrame& frame);

    void ApplyHover(UIFrame& frame, HitKey hoveredKey);
    bool TryCommit(HitKey clickedKey, int& outIndex) const;

    bool Visible() const { return visible_; }
    void SetVisible(bool v) { visible_ = v; }

    void SetKbFallbackEnabled(bool enabled) { kbFallbackEnabled_ = enabled; }

private:
    ChoiceHudConfig cfg_{};
    bool visible_ = false;

    int selectedOption_ = 0;
    int optionCount_ = 0;

    bool kbFallbackEnabled_ = false;

    std::vector<Render::RectF> itemRects_;
    std::vector<int> idxItemSprite_;
    std::vector<int> idxItemText_;
};

} // namespace Salt2D::Game::UI

#endif // GAME_UI_WIDGETS_CHOICE_DIALOG_WIDGET_H
