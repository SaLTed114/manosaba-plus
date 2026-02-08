// Game//UI/Widgets/DebateDialogWidget.h
#ifndef GAME_UI_WIDGETS_DEBATEDIALOGWIDGET_H
#define GAME_UI_WIDGETS_DEBATEDIALOGWIDGET_H

#include "Game/UI/Framework/UIFrame.h"
#include "Game/UI/Framework/UIBuilder.h"
#include "Game/UI/UITypes.h"

#include <string>
#include <vector>
#include <utility>

namespace Salt2D::Game::UI {

struct DebateHudConfig {
    Render::Color4F bodyTint     {1,1,1,1};
    Render::Color4F susTint      {1,0.45f,0.45f,1};
    Render::Color4F susHoverTint {1,0.70f,0.70f,1};

    float lineGap = 6.0f;
};

class DebateDialogWidget {
public:
    void SetConfig(DebateHudConfig cfg) { cfg_ = cfg; }
    void Build(const DebateHudModel& model, uint32_t canvasW, uint32_t canvasH, UIFrame& frame);
    void AfterBake(UIFrame& frame);

    void ApplyHover(UIFrame& frame, HitKey hoveredKey);
    bool TryPickSpan(HitKey clickedKey, int& outSpanIndex) const;

    bool Visible() const { return visible_; }
    void SetVisible(bool v) { visible_ = v; }

private:
    struct Piece {
        int textIdx = -1;
        bool isSus = false;
        int spanIdx = -1;
        int hitIdx = -1;
    };

private:
    DebateHudConfig cfg_{};
    bool visible_ = false;

    std::vector<Piece> pieces_;
    std::vector<int> lineBegin_;
    std::vector<int> lineCount_;

    int spanCount_ = 0;

    float baseX_ = 0.0f;
    float baseY_ = 0.0f;
};

} // namespace Salt2D::Game::UI

#endif // GAME_UI_WIDGETS_DEBATEDIALOGWIDGET_H
