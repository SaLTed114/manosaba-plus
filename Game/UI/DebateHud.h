// Game/UI/DebateHud.h
#ifndef GAME_UI_DEBATEHUD_H
#define GAME_UI_DEBATEHUD_H

#include <string>
#include <vector>
#include <cstdint>
#include "Render/Draw/SpriteDrawItem.h"
#include "Game/UI/UITypes.h"

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
};

struct DebateHudModel {
    bool visible = false;
    std::string speakerUtf8;
    std::string bodyUtf8;
    
    std::vector<std::string> spanIds;

    bool menuOpen = false;
    std::string openedSpanId;
    // {optionId, label}
    std::vector<std::pair<std::string, std::string>> menuOptions;
    
    int selectedSpan = 0;
    int selectedOpt  = 0;
};

struct DebateHudDrawData {
    bool visible = false;
    Render::RectF panel{};
    Render::Color4F panelTint{};
    TextRequest speaker{};
    TextRequest body{};

    std::vector<TextRequest> spans;
    std::vector<TextRequest> options;

    std::vector<Render::RectF> highlightRects;
    Render::Color4F highlightTint{1.0f, 1.0f, 0.0f, 0.25f};
};

class DebateHud {
public:
    void SetConfig(DebateHudConfig cfg) { cfg_ = cfg; }
    const DebateHudDrawData& Build(const DebateHudModel& model, uint32_t canvasW, uint32_t canvasH);
private:
    DebateHudConfig cfg_{};
    DebateHudDrawData drawData_{};
};

} // namespace Salt2D::Game::UI

#endif // GAME_UI_DEBATEHUD_H
