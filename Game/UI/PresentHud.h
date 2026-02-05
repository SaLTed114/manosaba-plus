// Game/UI/PresentHud.h
#ifndef GAME_UI_PRESENTHUD_H
#define GAME_UI_PRESENTHUD_H

#include <string>
#include <vector>
#include <cstdint>
#include "Render/Draw/SpriteDrawItem.h"
#include "Game/UI/UITypes.h"

namespace Salt2D::Game::UI {

struct PresentHudConfig {
    float margin    = 24.0f;
    float pad       = 8.0f;

    float panelW    = 560.0f;
    float promptH   = 48.0f;
    float promptGap = 10.0f;

    float rowH      = 36.0f;
    float rowGap    = 6.0f;

    Render::Color4F panelTint{0.0f, 0.0f, 0.0f, 0.55f};
    Render::Color4F highlightTint{1.0f, 1.0f, 0.0f, 0.25f};
};

struct PresentHudModel {
    bool visible = false;
    std::string promptUtf8;

    // {itemId, label}
    std::vector<std::pair<std::string, std::string>> items;
    
    int selectedItem = 0;
};

struct PresentHudDrawData {
    bool visible = false;

    Render::RectF panel{};
    Render::Color4F panelTint{};

    TextRequest prompt{};
    std::vector<TextRequest> items;

    std::vector<Render::RectF> highlightRects;
    Render::Color4F highlightTint{};
};

class PresentHud {
public:
    void SetConfig(const PresentHudConfig& cfg) { cfg_ = cfg; }
    const PresentHudDrawData& Build(const PresentHudModel& model, uint32_t canvasW, uint32_t canvasH);
    
private:
    PresentHudConfig cfg_;
    PresentHudDrawData draw_;
};

} // namespace Salt2D::Game::UI

#endif // GAME_UI_PRESENTHUD_H
