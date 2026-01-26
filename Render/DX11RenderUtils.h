// Render/DX11RenderUtils.h
#ifndef RENDER_DX11_RENDER_UTILS_H
#define RENDER_DX11_RENDER_UTILS_H

#include <cstdint>
#include <d3d11.h>

namespace Salt2D::Render {

inline D3D11_VIEWPORT MakeViewport(uint32_t w, uint32_t h) {
    D3D11_VIEWPORT viewport = {};
    viewport.TopLeftX = 0.0f; viewport.TopLeftY = 0.0f;
    viewport.Width    = static_cast<FLOAT>(w);
    viewport.Height   = static_cast<FLOAT>(h);
    viewport.MinDepth = 0.0f; viewport.MaxDepth = 1.0f;
    return viewport;
}

} // namespace Salt2D::Render

#endif // RENDER_DX11_RENDER_UTILS_H
