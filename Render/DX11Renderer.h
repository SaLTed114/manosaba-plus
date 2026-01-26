// Render/DX11Renderer.h
#ifndef RENDER_DX11RENDERER_H
#define RENDER_DX11RENDERER_H

#include <cstdint>
#include <wrl/client.h>

#include "RHI/DX11/DX11Device.h"
#include "RHI/DX11/DX11SwapChain.h"
#include "RHI/DX11/DX11Texture2D.h"
#include "Render/Shader/ShaderManager.h"
#include "Render/Draw/DrawList.h"
#include "Render/Sprite/SpriteRenderer.h"

namespace Salt2D::Render {

class DX11Renderer {
public:
    DX11Renderer(HWND hwnd, uint32_t width, uint32_t height);

    void Resize(uint32_t width, uint32_t height);

    void RenderFrame(bool vsync = true);

private:
    void BeginFrame();
    void EndFrame(bool vsync);
    void UpdateViewport();

private:
    uint32_t width_  = 0;
    uint32_t height_ = 0;

    RHI::DX11::DX11Device       device_;
    RHI::DX11::DX11SwapChain    swapChain_;

    ShaderManager     shaderManager_;
    DrawList          drawList_;
    SpriteRenderer    spriteRenderer_;

    RHI::DX11::DX11Texture2D testTexture_;
};

} // namespace Salt2D::Render

#endif // RENDER_DX11RENDERER_H
