// Render/DX11Renderer.cpp
#include "DX11Renderer.h"
#include "Utils/FileUtils.h"
#include "Resources/Image/WICImageLoader.h"

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
      device_(), swapChain_(device_, hwnd, width, height)
{
    std::vector<std::filesystem::path> searchPaths;

    if (auto shaderDir = Utils::TryResolvePath("Shaders")) {
        searchPaths.push_back(*shaderDir);
    }
    
    if (auto buildShaderDir = Utils::TryResolvePath("Build/Render/Shaders")) {
        searchPaths.push_back(*buildShaderDir);
    }

    if (searchPaths.empty()) throw std::runtime_error("Failed to locate shader directories.");

    shaderManager_.Initialize(std::move(searchPaths));

    float factor = 1.0f;

    internalW_ = static_cast<uint32_t>(width_ * factor);
    internalH_ = static_cast<uint32_t>(height_ * factor);

    sceneRT_ = RHI::DX11::DX11Texture2D::CreateRenderTarget(device_, internalW_, internalH_);
    compose_.Initialize(device_, shaderManager_);

    const uint32_t texW = 256;
    const uint32_t texH = 256;
    auto rgba = MakeCheckerRGBA(texW, texH);
    testTexture_ = RHI::DX11::DX11Texture2D::CreateRGBA8(device_, texW, texH, rgba.data(), texW * 4);

    
    Resources::ImageRGBA8 img;
    if (!Resources::LoadImageRGBA8_WIC("Assets/Textures/01.png", img)) {
        throw std::runtime_error("Failed to load image: Assets/Textures/01.png");
    }

    imgTexture_ = RHI::DX11::DX11Texture2D::CreateRGBA8(
        device_, img.width, img.height,
        img.pixels.data(), img.rowPitch
    );

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
    drawList_.PushSprite(Layer::Background, srv, RectF{0,0,(float)width_,(float)height_}, 0.0f);
    drawList_.PushSprite(Layer::HUD, srv, RectF{20,20,256,256}, 0.0f);

    auto imgSrv = imgTexture_.SRV();
    drawList_.PushSprite(Layer::Stage, imgSrv, RectF{100,100, (float)imgTexture_.GetWidth(), (float)imgTexture_.GetHeight()}, 0.0f);

    drawList_.Sort();
}

void DX11Renderer::BeginScenePass() {
    auto ctx = device_.GetContext();

    ID3D11RenderTargetView* rtvs[] = { sceneRT_.RTV() };
    ctx->OMSetRenderTargets(1, rtvs, nullptr);

    auto vp = MakeViewport(internalW_, internalH_);
    ctx->RSSetViewports(1, &vp);

    float clearColor[4] = {0.2f, 0.2f, 0.2f, 1.0f};
    ctx->ClearRenderTargetView(sceneRT_.RTV(), clearColor);
}

void DX11Renderer::ComposeToBackBuffer() {
    auto ctx = device_.GetContext();

    auto backRTV = swapChain_.GetBackBufferRTV();
    ID3D11RenderTargetView* rtvs[] = { backRTV.Get() };
    ctx->OMSetRenderTargets(1, rtvs, nullptr);

    auto vp = MakeViewport(width_, height_);
    ctx->RSSetViewports(1, &vp);

    compose_.Draw(device_, sceneRT_.SRV());
}

void DX11Renderer::Present(bool vsync) {
    swapChain_.Present(vsync);
}

void DX11Renderer::RenderFrame(bool vsync) {
    if (width_ == 0 || height_ == 0) return;
    if (internalW_ == 0 || internalH_ == 0) return;

    BuildTestDrawList(); // TMP

    BeginScenePass();

    auto sceneSprites = drawList_.Sprites(Layer::Background, Layer::Text);
    spriteRenderer_.Draw(device_, sceneSprites, width_, height_);

    ComposeToBackBuffer();

    auto hudSprites = drawList_.Sprites(Layer::HUD);
    spriteRenderer_.Draw(device_, hudSprites, width_, height_);

    Present(vsync);
}

} // namespace Salt2D::Render
