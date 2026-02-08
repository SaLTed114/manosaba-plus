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
    float margin       = 24.0f;
    float pad          = 16.0f;
    
    float panelW       = 520.0f;
    float speakerH     = 40.0f;
    float speakerGap   = 8.0f;

    float selectionGap = 12.0f;
    float rowH         = 36.0f;

    Render::Color4F panelTint{0.0f, 0.0f, 0.0f, 0.55f};

    // statement 区域（屏幕中间的大字区域）
    RectScale bodyRectScale { 0.18f, 0.28f, 0.64f, 0.50f };
    float lineGapPx = 10.0f;

    Render::Color4F bodyTint     {1,1,1,1};
    Render::Color4F susTint      {1,0.45f,0.45f,1};
    Render::Color4F susHoverTint {1,0.70f,0.70f,1};

    // menu 区域（点开可疑点后的选项框）
    RectScale menuRectScale { 0.22f, 0.35f, 0.56f, 0.22f };
    float menuPadPx = 18.0f;
    float optionH   = 54.0f;
    float optionGap = 12.0f;

    TextureId menuBgTex = TextureId::White;
    Render::Color4F menuBgTint {0,0,0,0.55f};

    TextureId btnTex = TextureId::White;
    Render::Color4F btnTint      {1,1,1,0.25f};
    Render::Color4F btnHoverTint {1,0.4f,0.4f,0.45f};

    Render::Color4F optTextTint {1,1,1,1};
};

class DebateDialogWidget {
public:
    void SetConfig(DebateHudConfig cfg) { cfg_ = cfg; }
    void Build(const DebateHudModel& model, uint32_t canvasW, uint32_t canvasH, UIFrame& frame);
    void AfterBake(UIFrame& frame);

    void ApplyHover(UIFrame& frame, HitKey hoveredKey);

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

    float baseX_ = 0.0f;
    float baseY_ = 0.0f;
    float lineGap_ = 6.0f;
};

} // namespace Salt2D::Game::UI

#endif // GAME_UI_WIDGETS_DEBATEDIALOGWIDGET_H
