// Game//UI/Widgets/DebateDialogWidget.h
#ifndef GAME_UI_WIDGETS_DEBATEDIALOGWIDGET_H
#define GAME_UI_WIDGETS_DEBATEDIALOGWIDGET_H

#include "Game/UI/Framework/UIFrame.h"
#include "Game/UI/Framework/UIBuilder.h"
#include "Game/UI/UITypes.h"
#include "Game/Story/TextMarkup/SusMarkup.h"

#include <string>
#include <vector>
#include <utility>
#include <unordered_map>

namespace Salt2D::Game::UI {

struct DebateHudConfig {
    Render::Color4F bodyTint     {1,1,1,1};
    Render::Color4F susTint      {1,0.45f,0.45f,1};
    Render::Color4F susHoverTint {1,0.70f,0.70f,1};

    float lineGap = 6.0f;
};

struct TransparentStringHash {
    using is_transparent = void;
    size_t operator()(const std::string& s) const {
        return std::hash<std::string>{}(s);
    }
    size_t operator()(std::string_view sv) const {
        return std::hash<std::string_view>{}(sv);
    }
};

struct TransparentStringEqual {
    using is_transparent = void;
    bool operator()(const std::string& a, const std::string& b) const { return a == b; }
    bool operator()(const std::string& a, std::string_view b) const { return std::string_view(a) == b; }
    bool operator()(std::string_view a, const std::string& b) const { return a == std::string_view(b); }
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

    struct LayoutRegion {
        float x0 = 0.0f, y0 = 0.0f;
        float w  = 0.0f, h  = 0.0f;
    };

private:
    LayoutRegion ComputeLayout(uint32_t canvasW, uint32_t canvasH);

    void EnsureLine(int lineIdx);
    void AddPieceToLine(int lineIdx, const Piece& piece);

    void PushParsedRuns(UIFrame& frame, const DebateHudModel& model,
        const LayoutRegion& region, const Story::SusParseResult& parsed);
    void PushRun(UIFrame& frame, const DebateHudModel& model,
        const LayoutRegion& region, const Story::SusRun& run, int& ioLine);
    void PushSegment(UIFrame& frame, const DebateHudModel& model, const LayoutRegion& region,
        const std::string& segUtf8, bool isSus, std::string_view spanId, int lineIdx);

private:
    DebateHudConfig cfg_{};
    bool visible_ = false;

    std::vector<Piece> pieces_;
    std::vector<int> lineBegin_;
    std::vector<int> lineCount_;

    int spanCount_ = 0;

    float alpha_  = 1.0f;
    float baseX_  = 0.0f;
    float baseY_  = 0.0f;
    float rotRad_ = 0.0f;

    std::unordered_map<std::string, int,
        TransparentStringHash, TransparentStringEqual> spanMap_;
};

} // namespace Salt2D::Game::UI

#endif // GAME_UI_WIDGETS_DEBATEDIALOGWIDGET_H
