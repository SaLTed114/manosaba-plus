// Game/RenderBridge/TextureCatalog.cpp
#include "TextureCatalog.h"

#include "Resources/Image/WICImageLoader.h"
#include "Utils/StringUtils.h"
#include <algorithm>

namespace Salt2D::Game::RenderBridge {

static inline std::filesystem::path LexicallyNormalize(const std::filesystem::path& p) {
    return p.lexically_normal();
}

std::string TextureCatalog::NormalizeKey(std::string_view relPathUtf8) {
    std::string str(relPathUtf8);
    std::replace(str.begin(), str.end(), '\\', '/');

    std::filesystem::path p(str);
    p = p.lexically_normal();

    std::string key = p.generic_string();

    std::transform(key.begin(), key.end(), key.begin(), [](unsigned char c) {
        return static_cast<char>(std::tolower(c));
    });

    return key;
}

std::filesystem::path TextureCatalog::MakeFullPath(std::string_view relPathUtf8) const {
    std::filesystem::path relPath(relPathUtf8);
    relPath = LexicallyNormalize(relPath);
    return LexicallyNormalize(assetsRoot_ / relPath);
}

TextureRef TextureCatalog::Validate(const Entry& entry) const {
    TextureRef result;
    result.srv = entry.tex.SRV();
    result.w = entry.w;
    result.h = entry.h;
    result.valid = (entry.valid && result.srv != nullptr);
    return result.valid ? result : missing_;
}

TextureRef TextureCatalog::GetOrLoad(const RHI::DX11::DX11Device& device, std::string_view relPathUtf8) {
    if (relPathUtf8.empty()) return missing_;

    const std::string key = NormalizeKey(relPathUtf8);

    if (auto it = cache_.find(key); it != cache_.end()) {
        return Validate(it->second);
    }

    const std::filesystem::path fullPath = MakeFullPath(relPathUtf8);

    Resources::ImageRGBA8 img{};
    bool ok = false;
    try {
        ok = Resources::LoadImageRGBA8_WIC(fullPath, img);
    } catch (...) {
        ok = false;
    }

    if (!ok || img.pixels.empty() || img.width == 0 || img.height == 0) {
        if (logger_) {
            logger_->Warn("TextureCatalog", "Failed to load texture: " + fullPath.string());
        }

        // Cache the failure to avoid repeated attempts
        cache_.emplace(key, Entry{}); 
        return missing_;
    }

    Entry entry;
    entry.tex = RHI::DX11::DX11Texture2D::CreateRGBA8(
        device, img.width, img.height, img.pixels.data(), img.rowPitch);

    entry.w = img.width;
    entry.h = img.height;
    entry.valid = (entry.tex.SRV() != nullptr);

    cache_.emplace(key, std::move(entry));

    return Validate(cache_.find(key)->second);
}

} // namespace Salt2D::Game::RenderBridge
