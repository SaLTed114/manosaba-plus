// Game/UI/ChoiceHud.h
#ifndef GAME_UI_CHOICEHUD_H
#define GAME_UI_CHOICEHUD_H

#include <string>
#include <vector>
#include <cstdint>
#include "Render/Draw/SpriteDrawItem.h"
#include "Game/UI/UITypes.h"

namespace Salt2D::Game::UI {

struct ChoiceHudConfig {
    float margin    = 24.0f;
    float pad       = 8.0f;

    float panelW    = 560.0f;
    float optionH   = 36.0f;
    float optionGap = 6.0f;

    Render::Color4F panelTint{0.0f, 0.0f, 0.0f, 0.55f};
    Render::Color4F highlightTint{1.0f, 1.0f, 0.0f, 0.25f};
};

struct ChoiceHudModel {
    bool visible = false;
    
    // {optionId, label}
    std::vector<std::pair<std::string, std::string>> options;
    
    int selectedOption = 0;
};

struct ChoiceHudDrawData {
    bool visible = false;

    Render::RectF panel{};
    Render::Color4F panelTint{};

    std::vector<TextRequest> options;

    std::vector<Render::RectF> highlightRects;
    Render::Color4F highlightTint{};
};

class ChoiceHud {
public:
    void SetConfig(const ChoiceHudConfig& cfg) { cfg_ = cfg; }
    const ChoiceHudDrawData& Build(const ChoiceHudModel& model, uint32_t canvasW, uint32_t canvasH);

private:
    ChoiceHudConfig cfg_;
    ChoiceHudDrawData draw_;
};

} // namespace Salt2D::Game::UI

#endif // GAME_UI_CHOICEHUD_H
