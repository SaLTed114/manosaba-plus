// Game/Story/TextMarkup/SusMarkup.cpp
#include "SusMarkup.h"
#include <unordered_set>

namespace Salt2D::Game::Story {

static inline bool StartWith(std::string_view str, size_t pos, std::string_view pat) {
    return pos + pat.size() <= str.size() && str.substr(pos, pat.size()) == pat;
}

SusParseResult ParseSusMarkup(std::string_view src) {
    SusParseResult result;

    std::unordered_set<std::string> seen;
    std::string curText;

    auto FlushText = [&]() {
        if (!curText.empty()) {
            result.runs.push_back({ SusRun::Kind::Text, std::move(curText), "" });
            curText.clear();
        }
    };

    const std::string_view openPrefix = "{sus:";
    const std::string_view openSuffix = "}";
    const std::string_view closeTag = "{/sus}";

    size_t pos = 0;
    while (pos < src.size()) {
        if (!StartWith(src, pos, openPrefix)) {
            curText += src[pos++];
            continue;
        }

        FlushText();

        size_t idBegin = pos + openPrefix.size();
        size_t idEnd = src.find(openSuffix, idBegin);
        if (idEnd == std::string_view::npos) {
            result.ok = false;
            result.errorMsg = "Unclosed {sus: tag at position " + std::to_string(pos);
            return result;
        }

        std::string spanId(src.substr(idBegin, idEnd - idBegin));
        if (spanId.empty()) {
            result.ok = false;
            result.errorMsg = "Empty span ID in {sus: tag at position " + std::to_string(pos);
            return result;
        }

        if (seen.count(spanId) > 0) {
            result.ok = false;
            result.errorMsg = "Duplicate span ID '" + spanId + "' at position " + std::to_string(pos);
            return result;
        }

        seen.insert(spanId);
        result.spanIds.push_back(spanId);

        size_t contentBegin = idEnd + openSuffix.size();
        size_t contentEnd = src.find(closeTag, contentBegin);
        if (contentEnd == std::string_view::npos) {
            result.ok = false;
            result.errorMsg = "Unclosed {sus: tag for span ID '" + spanId + "' at position " + std::to_string(pos);
            return result;
        }

        size_t nested = src.find(openPrefix, contentBegin);
        if (nested != std::string_view::npos && nested < contentEnd) {
            result.ok = false;
            result.errorMsg = "Nested {sus: tags are not allowed (span ID '" + spanId + "' at position " + std::to_string(pos) + ")";
            return result;
        }

        std::string susText(src.substr(contentBegin, contentEnd - contentBegin));
        result.runs.push_back({ SusRun::Kind::Sus, std::move(susText), spanId });

        pos = contentEnd + closeTag.size();
    }

    FlushText();

    result.plainTextUtf8.clear();
    result.plainTextUtf8.reserve(src.size());
    for (const auto& run : result.runs) {
        result.plainTextUtf8 += run.textUtf8;
    }

    result.ok = true;
    return result;
}

} // namespace Salt2D::Game::Story
