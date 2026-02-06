// Game/RenderBridge/TextureService.h
#ifndef GAME_RENDERBRIDGE_TEXTURESERVICE_H
#define GAME_RENDERBRIDGE_TEXTURESERVICE_H

#include <array>
#include <cstdint>
#include <wrl/client.h>
#include <d3d11.h>

#include "Game/UI/UITypes.h"

namespace Salt2D::Game::RenderBridge {

struct TextureInfo {
    ID3D11ShaderResourceView* srv = nullptr;
    uint32_t w = 0;
    uint32_t h = 0;
    bool valid = false;
};

class TextureService {
public:
    void Clear();
    void Register(UI::TextureId id, ID3D11ShaderResourceView* srv, uint32_t w, uint32_t h);

    ID3D11ShaderResourceView* Get(UI::TextureId id);

    TextureInfo GetInfo(UI::TextureId id);

    void SetMissing(UI::TextureId id) { missingId_ = id; }

private:
    struct Entry {
        Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> srv;
        uint32_t w = 0;
        uint32_t h = 0;
        bool valid = false;
    };

    static size_t ToIndex(UI::TextureId id) { return static_cast<size_t>(id); }

private:
    std::array<Entry, 256> entries_;
    UI::TextureId missingId_ = static_cast<UI::TextureId>(0);
};

} // namespace Salt2D::Game::RenderBridge

#endif // GAME_RENDERBRIDGE_TEXTURESERVICE_H
