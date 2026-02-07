// Game/UI/UITypes.h
#ifndef GAME_UI_UITYPES_H
#define GAME_UI_UITYPES_H

#include <cstdint>
#include <string>
#include <vector>

#include "Render/Draw/SpriteDrawItem.h"

namespace Salt2D::Game::UI {

enum class TextureId : uint8_t {
    White = 0,
    Checker = 1,
    Count
};

enum class TextStyleId : uint8_t {
    VnSpeaker       = 0,
    VnBody          = 1,
    DebateSpeaker   = 2,
    DebateBody      = 3,
    DebateSmall     = 4,
    PresentPrompt   = 5,
    PresentSmall    = 6,
    ChoiceSmall     = 7,
    VnNameFamilyBig = 8,
    VnNameGivenBig  = 9,
    VnNameRest      = 10,
    Count
};

enum class HitKind : uint8_t {
    None = 0,
    ChoiceOption = 1,
    Count
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
    float pad        = 0.0f;
    float maxBoxH    = 540.0f;
    float boxHRatio  = 0.30f;

    float speakerH   = 120.0f;
    float speakerGap = 8.0f;

    float nameSegGap  = 2.0f;
    float namePartGap = 2.0f;
    Render::Color4F nameAccentTint{0.85f, 0.55f, 1.0f, 1.0f};

    Render::Color4F panelTint{0.0f, 0.0f, 0.0f, 0.55f};
};

struct VnHudModel {
    bool visible = false;
    std::string speakerUtf8;
    std::string bodyUtf8;
};

struct ChoiceHudConfig {
    float barWRatio = 0.60f;
    float barH      = 90.0f;
    float barGap    = 16.0f;

    Render::Color4F barTint{1.0f, 1.0f, 1.0f, 0.75f};
    Render::Color4F barHoverTint{1.0f, 0.4f, 0.4f, 0.85f};
    Render::Color4F textTint{1.0f, 1.0f, 1.0f, 1.0f};
    Render::Color4F textHoverTint{1.0f, 1.0f, 1.0f, 1.0f};
};

struct ChoiceHudModel {
    bool visible = false;
    
    // {optionId, label}
    std::vector<std::pair<std::string, std::string>> options;
    
    int selectedOption = 0;
};


using HitKey = uint32_t;

static constexpr HitKey MakeHitKey(HitKind kind, uint32_t index) {
    return (static_cast<uint32_t>(kind) << 24) | (index & 0xFFFFFF);
}
static constexpr HitKey MakeHitKey(HitKind kind, int index) {
    return MakeHitKey(kind, static_cast<uint32_t>(index)); 
}

static constexpr HitKind HitKeyKind(HitKey key) { return static_cast<HitKind>(key >> 24); }
static constexpr uint32_t HitKeyIndex(HitKey key) { return key & 0xFFFFFF; }

} // namespace Salt2D::Game::UI

#endif // GAME_UI_UITYPES_H
