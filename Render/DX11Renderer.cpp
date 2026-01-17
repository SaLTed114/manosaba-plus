// Render/DX11Renderer.cpp
#include "DX11Renderer.h"

namespace Salt2D::Render {

DX11Renderer::DX11Renderer(HWND hwnd, uint32_t width, uint32_t height)
    : width_(width), height_(height),
      device_(), swapChain_(device_, hwnd, width, height),
      shaderManager_({ std::filesystem::path("Build/Bin/Debug/Shaders") })
{
    UpdateViewport();
    triangleDemo_.Initialize(device_.GetDevice(), shaderManager_);
}

void DX11Renderer::Resize(uint32_t width, uint32_t height) {
    width_  = width;
    height_ = height;

    if (width_ == 0 || height_ == 0) {
        swapChain_.Resize(width_, height_);
        return;
    }

    ID3D11RenderTargetView* nullRTV = nullptr;
    device_.GetContext()->OMSetRenderTargets(1, &nullRTV, nullptr);

    swapChain_.Resize(width_, height_);
    UpdateViewport();
}

void DX11Renderer::BeginFrame() {
    ID3D11DeviceContext* context = device_.GetContext();
    auto rtv = swapChain_.GetBackBufferRTV();

    float clearColor[4] = {0.1f, 0.1f, 0.1f, 1.0f};
    ID3D11RenderTargetView* rtvs[] = { rtv.Get() };

    context->OMSetRenderTargets(1, rtvs, nullptr);
    context->ClearRenderTargetView(rtv.Get(), clearColor);

    triangleDemo_.Draw(context, rtv.Get());
}

void DX11Renderer::EndFrame(bool vsync) {
    swapChain_.Present(vsync);
}

void DX11Renderer::UpdateViewport() {
    D3D11_VIEWPORT viewport = {};
    viewport.TopLeftX = 0.0f;
    viewport.TopLeftY = 0.0f;
    viewport.Width    = static_cast<FLOAT>(width_);
    viewport.Height   = static_cast<FLOAT>(height_);
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;

    device_.GetContext()->RSSetViewports(1, &viewport);
}

} // namespace Salt2D::Render
