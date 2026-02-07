// Game/UI/Theme/TextTheme.h
#ifndef GAME_UI_THEME_TEXTTHEME_H
#define GAME_UI_THEME_TEXTTHEME_H

#include <array>
#include "Game/UI/UITypes.h"
#include "Render/Text/TextBaker.h"

namespace Salt2D::Game::UI {

struct TextTheme {
    std::array<Render::Text::TextStyle, static_cast<size_t>(TextStyleId::Count)> styles;

    void InitDefault() {
        auto& vnSpeaker = styles[static_cast<size_t>(TextStyleId::VnSpeaker)];
        vnSpeaker.fontFamily = L"SimSun";
        vnSpeaker.fontSize   = 28.0f;
        vnSpeaker.weight     = DWRITE_FONT_WEIGHT_BOLD;

        auto& vnBody = styles[static_cast<size_t>(TextStyleId::VnBody)];
        vnBody.fontFamily = L"SimSun";
        vnBody.fontSize   = 35.0f;
        vnBody.weight     = DWRITE_FONT_WEIGHT_SEMI_BOLD;

        auto& debateSpeaker = styles[static_cast<size_t>(TextStyleId::DebateSpeaker)];
        debateSpeaker.fontFamily = L"SimSun";
        debateSpeaker.fontSize   = 24.0f;
        debateSpeaker.weight     = DWRITE_FONT_WEIGHT_BOLD;

        auto& debateBody = styles[static_cast<size_t>(TextStyleId::DebateBody)];
        debateBody.fontFamily = L"SimSun";
        debateBody.fontSize   = 20.0f;
        debateBody.weight     = DWRITE_FONT_WEIGHT_REGULAR;

        auto& debateSmall = styles[static_cast<size_t>(TextStyleId::DebateSmall)];
        debateSmall.fontFamily = L"SimSun";
        debateSmall.fontSize   = 16.0f;
        debateSmall.weight     = DWRITE_FONT_WEIGHT_REGULAR;

        auto& presentPrompt = styles[static_cast<size_t>(TextStyleId::PresentPrompt)];
        presentPrompt.fontFamily = L"SimSun";
        presentPrompt.fontSize   = 20.0f;
        presentPrompt.weight     = DWRITE_FONT_WEIGHT_REGULAR;

        auto& presentSmall = styles[static_cast<size_t>(TextStyleId::PresentSmall)];
        presentSmall.fontFamily = L"SimSun";
        presentSmall.fontSize   = 18.0f;
        presentSmall.weight     = DWRITE_FONT_WEIGHT_REGULAR;

        auto& choiceSmall = styles[static_cast<size_t>(TextStyleId::ChoiceSmall)];
        choiceSmall.fontFamily = L"SimSun";
        choiceSmall.fontSize   = 40.0f;
        choiceSmall.weight     = DWRITE_FONT_WEIGHT_SEMI_BOLD;

        auto& vnNameFamilyBig = styles[static_cast<size_t>(TextStyleId::VnNameFamilyBig)];
        vnNameFamilyBig.fontFamily = L"SimSun";
        vnNameFamilyBig.fontSize   = 100.0f;
        vnNameFamilyBig.weight     = DWRITE_FONT_WEIGHT_BOLD;

        auto& vnNameGivenBig = styles[static_cast<size_t>(TextStyleId::VnNameGivenBig)];
        vnNameGivenBig.fontFamily = L"SimSun";
        vnNameGivenBig.fontSize   = 75.0f;
        vnNameGivenBig.weight     = DWRITE_FONT_WEIGHT_BOLD;

        auto& vnNameRest = styles[static_cast<size_t>(TextStyleId::VnNameRest)];
        vnNameRest.fontFamily = L"SimSun";
        vnNameRest.fontSize   = 50.0f;
        vnNameRest.weight     = DWRITE_FONT_WEIGHT_BOLD;

        auto& presentTitleBig = styles[static_cast<size_t>(TextStyleId::PresentTitleBig)];
        presentTitleBig.fontFamily = L"SimSun";
        presentTitleBig.fontSize   = 60.0f;
        presentTitleBig.weight     = DWRITE_FONT_WEIGHT_EXTRA_BOLD;

        auto& presentTitleRest = styles[static_cast<size_t>(TextStyleId::PresentTitleRest)];
        presentTitleRest.fontFamily = L"SimSun";
        presentTitleRest.fontSize   = 45.0f;
        presentTitleRest.weight     = DWRITE_FONT_WEIGHT_EXTRA_BOLD;

        auto& presentDetail = styles[static_cast<size_t>(TextStyleId::PresentDetail)];
        presentDetail.fontFamily = L"SimSun";
        presentDetail.fontSize   = 40.0f;
        presentDetail.weight     = DWRITE_FONT_WEIGHT_BOLD;

        auto& presentShowBig = styles[static_cast<size_t>(TextStyleId::PresentShowBig)];
        presentShowBig.fontFamily = L"SimSun";
        presentShowBig.fontSize   = 50.0f;
        presentShowBig.weight     = DWRITE_FONT_WEIGHT_EXTRA_BOLD;

        auto& presentShowRest = styles[static_cast<size_t>(TextStyleId::PresentShowRest)];
        presentShowRest.fontFamily = L"SimSun";
        presentShowRest.fontSize   = 40.0f;
        presentShowRest.weight     = DWRITE_FONT_WEIGHT_EXTRA_BOLD;
    }

    const Render::Text::TextStyle& GetStyle(TextStyleId id) const {
        return styles[static_cast<size_t>(id)];
    }
};

} // namespace Salt2D::Game::UI

#endif // GAME_UI_THEME_TEXTTHEME_H
