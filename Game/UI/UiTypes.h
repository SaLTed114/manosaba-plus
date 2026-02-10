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
    VnSpeaker,
    VnBody,
    DebateSpeaker,
    DebateBody,
    DebateSus,
    PresentPrompt,
    PresentSmall,
    ChoiceSmall,
    VnNameFamilyBig,
    VnNameGivenBig,
    VnNameRest,
    PresentTitleBig,
    PresentTitleRest,
    PresentShowBig,
    PresentShowRest,
    PresentDetail,
    Timer,
    Count
};

enum class HitKind : uint8_t {
    None = 0,
    ChoiceOption = 1,
    PresentItem  = 2,
    PresentShow  = 3,
    DebateSpan   = 4,
    DebateOption = 5,
    DebateBack   = 6,
    DebateSpeed  = 7,
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

struct VnHudModel {
    bool visible = false;
    std::string speakerUtf8;
    std::string bodyUtf8;
};

struct ChoiceHudModel {
    bool visible = false;
    
    // {optionId, label}
    std::vector<std::pair<std::string, std::string>> options;
    
    int selectedOption = 0;
};

struct PresentHudModel {
    bool visible = false;
    std::string promptUtf8;

    // {itemId, label}
    std::vector<std::pair<std::string, std::string>> items;
    
    int selectedItem = 0;
};

struct HudPose2D {
    float baseX  = 0.0f;
    float baseY  = 0.0f;
    float rotRad = 0.0f;
    float alpha  = 1.0f;
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

    float timeScale = 1.0f;

    HudPose2D dialogPose;
};

// ==============================
// Basic rect helper
// ==============================

struct RectScale { float x=0, y=0, w=0, h=0; };

inline Render::RectF RectXYWH(float x, float y, float w, float h) {return Render::RectF{x, y, w, h}; }

inline Render::RectF RectFromScale(const RectScale& s, uint32_t W, uint32_t H) {
    return Render::RectF{ s.x * W, s.y * H, s.w * W, s.h * H };
}

// ==============================
// Hit testing key encoding
// ==============================

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
