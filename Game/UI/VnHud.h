// Game/UI/VnHud.h
#ifndef GAME_UI_VNHUD_H
#define GAME_UI_VNHUD_H

#include <cstdint>
#include <cstring>
#include <string>
#include "Render/Draw/SpriteDrawItem.h"
#include "Game/UI/UITypes.h"

namespace Salt2D::Game::UI {

struct VnHudConfig {
    float margin     = 24.0f;
    float pad        = 16.0f;
    float maxBoxH    = 260.0f;
    float boxHRatio  = 0.30f;
    float speakerH   = 44.0f;
    float speakerGap = 8.0f;
    Render::Color4F panelTint{0.0f, 0.0f, 0.0f, 0.55f};
};

struct VnHudModel {
    bool visible = false;
    std::string speakerUtf8;
    std::string bodyUtf8;
};

struct VnHudDrawData {
    bool visible = false;
    Render::RectF panel{};
    Render::Color4F panelTint{};
    TextRequest speaker{};
    TextRequest body{};
};

class VnHud {
public:
    void SetConfig(VnHudConfig cfg) { cfg_ = cfg; }
    const VnHudDrawData& Build(const VnHudModel& model, uint32_t canvasW, uint32_t canvasH);

private:
    VnHudConfig cfg_{};
    VnHudDrawData drawData_{};
};

} // namespace Salt2D::Game::UI

#endif // GAME_UI_VNHUD_H
