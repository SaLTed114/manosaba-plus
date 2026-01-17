// Render/DX11Renderer.h
#ifndef RENDER_DX11RENDERER_H
#define RENDER_DX11RENDERER_H

#include <cstdint>
#include <wrl/client.h>

#include "RHI/DX11/DX11Device.h"
#include "RHI/DX11/DX11SwapChain.h"
#include "Render/Shader/ShaderManager.h"
#include "Render/Primitives/TriangleDemo.h"

namespace Salt2D::Render {

class DX11Renderer {
public:
    DX11Renderer(HWND hwnd, uint32_t width, uint32_t height);

    void Resize(uint32_t width, uint32_t height);

    void BeginFrame();
    void EndFrame(bool vsync = true);

private:
    void UpdateViewport();

private:
    uint32_t width_  = 0;
    uint32_t height_ = 0;

    RHI::DX11::DX11Device       device_;
    RHI::DX11::DX11SwapChain    swapChain_;

    ShaderManager   shaderManager_;
    TriangleDemo    triangleDemo_;
};

} // namespace Salt2D::Render

#endif // RENDER_DX11RENDERER_H
