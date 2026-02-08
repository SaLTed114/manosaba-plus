// Game/Story/TextMarkup/SusMarkup.h
#ifndef GAME_STORY_TEXTMARKUP_SUSMARKUP_H
#define GAME_STORY_TEXTMARKUP_SUSMARKUP_H

#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

namespace Salt2D::Game::Story {

struct SusRun {
    enum class Kind : uint8_t { Text, Sus };
    Kind kind;
    std::string textUtf8;
    std::string spanId;
};

struct SusParseResult {
    std::vector<SusRun> runs;
    std::vector<std::string> spanIds;
    std::string plainTextUtf8;

    bool ok = false;
    std::string errorMsg;
};

SusParseResult ParseSusMarkup(std::string_view src);

} // namespace Salt2D::Game::Story

#endif // GAME_STORY_TEXTMARKUP_SUSMARKUP_H
