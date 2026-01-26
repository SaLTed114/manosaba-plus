// Render/DX11Renderer.cpp
#include "DX11Renderer.h"
#include "Utils/FileUtils.h"
#include "Resources/Image/WICImageLoader.h"

#include "DX11RenderUtils.h"
#include "Render/Primitives/TestPattern.h"

namespace Salt2D::Render {

// ========================== Constructor ==========================

DX11Renderer::DX11Renderer(HWND hwnd, uint32_t width, uint32_t height)
    : width_(width), height_(height),
      device_(), swapChain_(device_, hwnd, width, height)
{
    InitShaderSearchPaths();
    InitSceneTargets(1.0f);
    InitPipelines();
    InitStates();
    
    InitDemoTextures();
}

// ========================== Begin of Init Functions ==========================

void DX11Renderer::InitShaderSearchPaths() {
    std::vector<std::filesystem::path> searchPaths;

    if (auto shaderDir = Utils::TryResolvePath("Shaders"))
        searchPaths.push_back(*shaderDir);

    if (auto buildShaderDir = Utils::TryResolvePath("Build/Render/Shaders"))
        searchPaths.push_back(*buildShaderDir);

    if (searchPaths.empty())
        throw std::runtime_error("Failed to locate shader directories.");

    shaderManager_.Initialize(std::move(searchPaths));
}

void DX11Renderer::InitSceneTargets(float factor) {
    internalW_ = static_cast<uint32_t>(width_  * factor);
    internalH_ = static_cast<uint32_t>(height_ * factor);

    sceneRT_    = RHI::DX11::DX11Texture2D::CreateRenderTarget(device_, internalW_, internalH_);
    sceneDepth_ = RHI::DX11::DX11DepthBuffer::Create(device_, internalW_, internalH_);
}

void DX11Renderer::InitPipelines() {
    compose_.Initialize(device_, shaderManager_);
    spriteRenderer_.Initialize(device_, shaderManager_);
}

void DX11Renderer::InitStates() {
    states_.Initialize(device_.GetDevice());
    stateCache_.Reset();
}

void DX11Renderer::InitDemoTextures() {
    // checker
    const uint32_t texW = 256, texH = 256;
    auto rgba = MakeCheckerRGBA(texW, texH);
    testTexture_ = RHI::DX11::DX11Texture2D::CreateRGBA8(device_, texW, texH, rgba.data(), texW * 4);

    // file image
    std::filesystem::path imgPath = "Assets/Textures/667.png";
    Resources::ImageRGBA8 img;
    if (!Resources::LoadImageRGBA8_WIC(imgPath, img))
        throw std::runtime_error("Failed to load image: " + imgPath.string());

    imgTexture_ = RHI::DX11::DX11Texture2D::CreateRGBA8(
        device_, img.width, img.height, img.pixels.data(), img.rowPitch
    );
}

// ========================== End of Init Functions ==========================

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
    sceneDepth_ = RHI::DX11::DX11DepthBuffer::Create(device_, internalW_, internalH_);
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

// ========================== Begin of Render Functions ==========================

void DX11Renderer::BeginPass(
    ID3D11RenderTargetView* rtv,
    ID3D11DepthStencilView* dsv,
    uint32_t vpW, uint32_t vpH,
    DepthMode depthMode, BlendMode blendMode
) {
    auto ctx = device_.GetContext();

    ID3D11RenderTargetView* rtvs[] = { rtv };
    ctx->OMSetRenderTargets(1, rtvs, dsv);

    auto vp = MakeViewport(vpW, vpH);
    ctx->RSSetViewports(1, &vp);

    stateCache_.SetDepthStencilState(ctx, DS(depthMode));
    stateCache_.SetBlendState(ctx, BS(blendMode));
}

void DX11Renderer::BeginScenePass() {
    BeginPass(sceneRT_.RTV(), sceneDepth_.DSV(),
        internalW_, internalH_, DepthMode::Off, BlendMode::Alpha);
    
    auto ctx = device_.GetContext();

    float clearColor[4] = {0.2f, 0.2f, 0.2f, 1.0f};
    ctx->ClearRenderTargetView(sceneRT_.RTV(), clearColor);
    sceneDepth_.Clear(ctx, 1.0f, 0);
}

void DX11Renderer::ComposeToBackBuffer() {
    BeginPass(swapChain_.GetBackBufferRTV().Get(), nullptr,
        width_, height_, DepthMode::Off, BlendMode::Off);

    compose_.Draw(device_, sceneRT_.SRV());
}

void DX11Renderer::BeginHUDPass() {
    BeginPass(swapChain_.GetBackBufferRTV().Get(), nullptr,
        width_, height_, DepthMode::Off, BlendMode::Alpha);
}

void DX11Renderer::Present(bool vsync) {
    swapChain_.Present(vsync);
}

// ========================== End of Render Functions ==========================


// ========================== Begin of Main Render Function ==========================

void DX11Renderer::RenderFrame(bool vsync) {
    if (width_ == 0 || height_ == 0) return;
    if (internalW_ == 0 || internalH_ == 0) return;

    BuildTestDrawList(); // TMP

    BeginScenePass();
    auto sceneSprites = drawList_.Sprites(Layer::Background, Layer::Text);
    spriteRenderer_.Draw(device_, sceneSprites, width_, height_);

    ComposeToBackBuffer();

    BeginHUDPass();
    auto hudSprites = drawList_.Sprites(Layer::HUD);
    spriteRenderer_.Draw(device_, hudSprites, width_, height_);

    Present(vsync);
}

// ========================== End of Main Render Function ==========================

} // namespace Salt2D::Render
