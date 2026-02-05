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
};

struct TextRequest {
    TextStyleId style;
    std::string textUtf8;
    float layoutW = 0.0f;
    float layoutH = 0.0f;
    float x = 0.0f;
    float y = 0.0f;
};

} // namespace Salt2D::Game::UI

#endif // GAME_UI_UITYPES_H
