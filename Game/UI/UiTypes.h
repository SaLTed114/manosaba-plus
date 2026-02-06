// Game/UI/UiTypes.h
#ifndef GAME_UI_UITYPES_H
#define GAME_UI_UITYPES_H

#include <cstdint>
#include <string>

namespace Salt2D::Game::UI {

enum class TextStyleId : uint8_t {
    VnSpeaker       = 0,
    VnBody          = 1,
    DebateSpeaker   = 2,
    DebateBody      = 3,
    DebateSmall     = 4,
    PresentPrompt   = 5,
    PresentSmall    = 6,
    ChoiceSmall     = 7,
};

struct TextRequest {
    TextStyleId style;
    std::string textUtf8;
    float layoutW = 0.0f;
    float layoutH = 0.0f;
    float x = 0.0f;
    float y = 0.0f;
};

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

} // namespace Salt2D::Game::UI

#endif // GAME_UI_UITYPES_H
