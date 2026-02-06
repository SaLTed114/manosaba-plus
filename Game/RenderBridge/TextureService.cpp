// Game/RenderBridge/TextureService.cpp
#include "TextureService.h"

namespace Salt2D::Game::RenderBridge {

void TextureService::Clear() {
    for (auto& entry : entries_) {
        entry.srv.Reset();
        entry.w = 0; entry.h = 0;
        entry.valid = false;
    }
}

void TextureService::Register(UI::TextureId id, ID3D11ShaderResourceView* srv, uint32_t w, uint32_t h) {
    auto& entry = entries_[ToIndex(id)];
    entry.srv = srv;
    entry.w = w;
    entry.h = h;
    entry.valid = true;
}

ID3D11ShaderResourceView* TextureService::Get(UI::TextureId id) {
    auto& entry = entries_[ToIndex(id)];
    if (entry.valid && entry.srv) return entry.srv.Get();

    const auto& missingEntry = entries_[ToIndex(missingId_)];
    if (missingEntry.valid && missingEntry.srv) return missingEntry.srv.Get();

    return nullptr;
}

TextureInfo TextureService::GetInfo(UI::TextureId id) {
    auto& entry = entries_[ToIndex(id)];
    if (entry.valid && entry.srv) {
        return TextureInfo{entry.srv.Get(), entry.w, entry.h, true};
    }

    const auto& missingEntry = entries_[ToIndex(missingId_)];
    if (missingEntry.valid && missingEntry.srv) {
        return TextureInfo{missingEntry.srv.Get(), missingEntry.w, missingEntry.h, true};
    }

    return TextureInfo{};
}

} // namespace Salt2D::Game::RenderBridge
