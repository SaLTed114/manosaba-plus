// Render/DX11Renderer.cpp
#include "DX11Renderer.h"

namespace Salt2D::Render {

namespace {

static std::vector<uint8_t> MakeCheckerRGBA(uint32_t w, uint32_t h) {
    std::vector<uint8_t> img(w * h * 4);
    for (uint32_t y = 0; y < h; ++y) {
        for (uint32_t x = 0; x < w; ++x) {
            bool c = ((x / 16) ^ (y / 16)) & 1;
            uint8_t v = c ? 255 : 30;

            size_t idx = (y * w + x) * 4;
            img[idx + 0] = v;
            img[idx + 1] = (uint8_t)(x * 255 / (w - 1));
            img[idx + 2] = (uint8_t)(y * 255 / (h - 1));
            img[idx + 3] = 255;
        }
    }
    return img;
}

static inline D3D11_VIEWPORT MakeViewport(uint32_t w, uint32_t h) {
    D3D11_VIEWPORT viewport = {};
    viewport.TopLeftX = 0.0f; viewport.TopLeftY = 0.0f;
    viewport.Width    = static_cast<FLOAT>(w);
    viewport.Height   = static_cast<FLOAT>(h);
    viewport.MinDepth = 0.0f; viewport.MaxDepth = 1.0f;
    return viewport;
}

} // Anonymous namespace

DX11Renderer::DX11Renderer(HWND hwnd, uint32_t width, uint32_t height)
    : width_(width), height_(height),
      device_(), swapChain_(device_, hwnd, width, height),
      shaderManager_({ std::filesystem::path("Build/Bin/Debug/Shaders") })
{
    internalW_ = width_;
    internalH_ = height_;

    sceneRT_ = RHI::DX11::DX11Texture2D::CreateRenderTarget(device_, internalW_, internalH_);
    compose_.Initialize(device_, shaderManager_);

    const uint32_t texW = 256;
    const uint32_t texH = 256;
    auto rgba = MakeCheckerRGBA(texW, texH);
    testTexture_ = RHI::DX11::DX11Texture2D::CreateRGBA8(device_, texW, texH, rgba.data(), texW * 4);

    spriteRenderer_.Initialize(device_, shaderManager_);
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

    internalW_ = width_;
    internalH_ = height_;
    sceneRT_ = RHI::DX11::DX11Texture2D::CreateRenderTarget(device_, internalW_, internalH_);
}

void DX11Renderer::BuildTestDrawList() {
    drawList_.Clear();
    drawList_.ReserveSprites(8);

    auto srv = testTexture_.SRV();
    drawList_.PushSprite(Layer::Background, srv, RectF{0,0,(float)(width_ / 2),(float)(height_ / 2)}, 0.0f);
    drawList_.PushSprite(Layer::HUD, srv, RectF{20,20,256,256}, 0.0f);
    drawList_.Sort();
}

void DX11Renderer::BeginFrame() {
    auto context = device_.GetContext();
    
    ID3D11RenderTargetView* rtvs[] = { sceneRT_.RTV() };
    context->OMSetRenderTargets(1, rtvs, nullptr);

    auto viewport = MakeViewport(internalW_, internalH_);
    context->RSSetViewports(1, &viewport);

    float clearColor[4] = {0.2f, 0.2f, 0.2f, 1.0f};
    context->ClearRenderTargetView(sceneRT_.RTV(), clearColor);
}

void DX11Renderer::EndFrame(bool vsync) {
    auto context = device_.GetContext();
    auto backRTV = swapChain_.GetBackBufferRTV();

    ID3D11RenderTargetView* rtvs[] = { backRTV.Get() };
    context->OMSetRenderTargets(1, rtvs, nullptr);

    auto viewport = MakeViewport(width_, height_);
    context->RSSetViewports(1, &viewport);

    compose_.Draw(device_, sceneRT_.SRV());

    swapChain_.Present(vsync);
}

void DX11Renderer::RenderFrame(bool vsync) {
    if (width_ == 0 || height_ == 0) return;

    BeginFrame();

    BuildTestDrawList(); // TMP

    spriteRenderer_.Draw(device_, drawList_, internalW_, internalH_);

    EndFrame(vsync);
}

} // namespace Salt2D::Render
