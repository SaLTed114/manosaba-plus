// Game/UI/Widgets/HistoryWidget.h
#ifndef GAME_UI_WIDGETS_HISTORYWIDGET_H
#define GAME_UI_WIDGETS_HISTORYWIDGET_H

#include "Game/Story/StoryView.h"
#include "Game/UI/Framework/UIFrame.h"
#include "Game/UI/Framework/UIBuilder.h"
#include "Game/UI/UITypes.h"
#include "Game/Session/StoryHistory.h"
#include "UIButtonWidget.h"

#include <vector>
#include <span>

namespace Salt2D::Game::UI {

struct HistoryModel {
    bool active = false;
    std::span<const Session::HistoryEntry> entries;
};

struct HistoryConfig {
    RectScale rectScale{0.0f, 0.0f, 1.0f, 1.0f};
    RectScale closeBtnRect{0.9f, 0.05f, 0.08f, 0.05f};

    Render::Color4F textTint{1.0f, 1.0f, 1.0f, 1.0f};
    Render::Color4F panelTint{0.0f, 0.0f, 0.0f, 0.6f};
    Render::Color4F closeBtnTint{0.8f, 0.8f, 0.8f, 0.8f};
    Render::Color4F closeBtnHoverTint{1.0f, 0.4f, 0.4f, 0.9f};
};

class HistoryWidget {
public:
    void SetConfig(const HistoryConfig& cfg) { cfg_ = cfg; }
    void Build(const HistoryModel& model, uint32_t canvasW, uint32_t canvasH, UIFrame& frame);
    void AfterBake(UIFrame& frame);

    void ApplyHover(UIFrame& frame, HitKey hoveredKey);
    bool TryClose(HitKey clickedKey) const;

    bool Visible() const { return visible_; }
    void SetVisible(bool v) { visible_ = v; }

private:
    HistoryConfig cfg_{};
    bool visible_ = false;

    UIButtonWidget closeBtn_;
};

} // namespace Salt2D::Game::UI

#endif // GAME_UI_WIDGETS_HISTORYWIDGET_H
