// Render/DX11Renderer.cpp
#include "DX11Renderer.h"
#include "Utils/FileUtils.h"

#include "DX11RenderUtils.h"

namespace Salt2D::Render {

// ========================== Constructor ==========================

DX11Renderer::DX11Renderer(HWND hwnd, uint32_t width, uint32_t height)
    : canvasW_(width), canvasH_(height),
      device_(), swapChain_(device_, hwnd, width, height)
{
    internalScale_ = 1.0f;

    InitShaderSearchPaths();
    InitSceneTargets(internalScale_);
    InitPipelines();
    InitStates();

    cubeDemo_.Initialize(device_, shaderManager_);
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
    sceneW_ = static_cast<uint32_t>(canvasW_  * factor);
    sceneH_ = static_cast<uint32_t>(canvasH_ * factor);

    sceneRT_    = RHI::DX11::DX11Texture2D::CreateRenderTarget(device_, sceneW_, sceneH_);
    sceneDepth_ = RHI::DX11::DX11DepthBuffer::Create(device_, sceneW_, sceneH_);
}

void DX11Renderer::InitPipelines() {
    compose_.Initialize(device_, shaderManager_);
    spriteRenderer_.Initialize(device_, shaderManager_);
}

void DX11Renderer::InitStates() {
    states_.Initialize(device_.GetDevice());
    stateCache_.Reset();
}

// ========================== End of Init Functions ==========================

void DX11Renderer::Resize(uint32_t width, uint32_t height) {
    canvasW_  = width;
    canvasH_ = height;

    if (canvasW_ == 0 || canvasH_ == 0) {
        swapChain_.Resize(canvasW_, canvasH_);
        return;
    }

    ID3D11RenderTargetView* nullRTV = nullptr;
    device_.GetContext()->OMSetRenderTargets(1, &nullRTV, nullptr);

    swapChain_.Resize(canvasW_, canvasH_);

    sceneW_ = canvasW_;
    sceneH_ = canvasH_;
    sceneRT_ = RHI::DX11::DX11Texture2D::CreateRenderTarget(device_, sceneW_, sceneH_);
    sceneDepth_ = RHI::DX11::DX11DepthBuffer::Create(device_, sceneW_, sceneH_);
}

// ========================== Begin of Render Functions ==========================

void DX11Renderer::Present(bool vsync) {
    swapChain_.Present(vsync);
}

void DX11Renderer::ExecutePlan(const RenderPlan& plan, const FrameBlackboard& frame) {
    auto ctx = device_.GetContext();

    PassContext passCtx{
        .device  = device_,
        .ctx = ctx,

        .sceneRTV = sceneRT_.RTV(),
        .sceneSRV = sceneRT_.SRV(),
        .sceneDSV = sceneDepth_.DSV(),
        .backRTV  = swapChain_.backBufferRTV(),

        .canvasW = canvasW_,
        .canvasH = canvasH_,
        .sceneW  = sceneW_,
        .sceneH  = sceneH_,

        .states = states_,
        .cache  = stateCache_,

        .spriteRenderer = &spriteRenderer_,
        .compose        = &compose_,
        .cubeDemo       = &cubeDemo_,

        .frame = &frame,
    };

    for (const auto& pass : plan.passes) {
        pass->Record(passCtx);
    }
}

// ========================== End of Render Functions ==========================

} // namespace Salt2D::Render
