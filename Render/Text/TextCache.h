// Render/Text/TextCache.h
#ifndef RENDER_TEXT_TEXTCACHE_H
#define RENDER_TEXT_TEXTCACHE_H

#include <string>
#include <unordered_map>
#include <cstdint>

#include "TextBaker.h"
#include "RHI/DX11/DX11Device.h"
#include "Utils/StringUtils.h"

namespace Salt2D::Render::Text {

struct TextCacheKey {
    uint8_t styleId = 0;
    uint32_t w100 = 0; // layout width  * 100
    uint32_t h100 = 0; // layout height * 100
    std::string text;

    bool operator==(const TextCacheKey& other) const {
        return
            styleId == other.styleId &&
            w100 == other.w100 &&
            h100 == other.h100 &&
            text == other.text;
    }
};

struct TextCacheKeyHash {
    size_t operator()(const TextCacheKey& key) const noexcept {
        size_t h1 = std::hash<uint8_t>()(key.styleId);
        size_t h2 = std::hash<uint32_t>()(key.w100) + 0x9e3779b9 + (h1 << 6) + (h1 >> 2);
        size_t h3 = std::hash<uint32_t>()(key.h100) + 0x9e3779b9 + (h2 << 6) + (h2 >> 2);
        size_t h4 = std::hash<std::string>()(key.text) + 0x9e3779b9 + (h3 << 6) + (h3 >> 2);
        return h1 ^ (h2 << 1) ^ (h3 << 2) ^ (h4 << 3);
    }
};

class TextCache {
public:
    const BakedText& GetOrBake(
        const RHI::DX11::DX11Device& device,
        TextBaker& baker,
        uint8_t styleId, const TextStyle& style,
        const std::string& textUtf8,
        float layoutW, float layoutH
    ) {
        TextCacheKey key;
        key.styleId = styleId;
        key.w100 = static_cast<uint32_t>(layoutW * 100.0f + 0.5f);
        key.h100 = static_cast<uint32_t>(layoutH * 100.0f + 0.5f);
        key.text = textUtf8;

        auto it = cache_.find(key);
        if (it != cache_.end()) { return it->second; }

        std::wstring textW = Utils::Utf8ToWString(textUtf8);
        BakedText baked = baker.BakeToTexture(device, textW, style, layoutW, layoutH);

        auto [newIt, inserted] = cache_.emplace(std::move(key), std::move(baked));
        return newIt->second;
    }

    void Clear() { cache_.clear(); }

private:
    std::unordered_map<TextCacheKey, BakedText, TextCacheKeyHash> cache_;

};

} // namespace Salt2D::Render::Text

#endif // RENDER_TEXT_TEXTCACHE_H
