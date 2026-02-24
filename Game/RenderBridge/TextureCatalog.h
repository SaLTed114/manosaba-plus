// Game/RenderBridge/TextureCatalog.h
#ifndef GAME_RENDERBRIDGE_TEXTURECATALOG_H
#define GAME_RENDERBRIDGE_TEXTURECATALOG_H

#include <filesystem>
#include <string>
#include <string_view>
#include <unordered_map>

#include "Utils/Logger.h"
#include "RHI/DX11/DX11Texture2D.h"

struct ID3D11ShaderResourceView;

namespace Salt2D::Game::RenderBridge {

struct TextureRef {
    ID3D11ShaderResourceView* srv = nullptr;
    uint32_t w = 0;
    uint32_t h = 0;
    bool valid = false;
};

class TextureCatalog {
public:
    void SetAssetsRoot(std::filesystem::path root) { assetsRoot_ = std::move(root); }
    const std::filesystem::path& AssetsRoot() const { return assetsRoot_; }

    void SetMissing(TextureRef ref) { missing_ = ref; }
    void SetLogger(Utils::Logger* logger) { logger_ = logger; }

    TextureRef GetOrLoad(const RHI::DX11::DX11Device& device, std::string_view relPathUtf8);

    void Clear() { cache_.clear(); }

private:
    struct Entry {
        RHI::DX11::DX11Texture2D tex;
        uint32_t w = 0;
        uint32_t h = 0;
        bool valid = false;
    };

    static std::string NormalizeKey(std::string_view relPathUtf8);
    std::filesystem::path MakeFullPath(std::string_view relPathUtf8) const;

    TextureRef Validate(const Entry& entry) const;

private:
    std::filesystem::path assetsRoot_;
    TextureRef missing_{};

    std::unordered_map<std::string, Entry> cache_;

    Utils::Logger* logger_ = nullptr;
};

} // namespace Salt2D::Game::RenderBridge

#endif // GAME_RENDERBRIDGE_TEXTURECATALOG_H
