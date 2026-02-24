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

    InitDebugLayer();
    InitShaderSearchPaths();
    InitSceneTargets(internalScale_);

    pipelines_.Initialize(device_, shaderManager_);
    draw_.Initialize(device_);

    states_.Initialize(device_.GetDevice());
    stateCache_.Reset();
}

DX11Renderer::~DX11Renderer() = default;

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

    sceneRT_[0] = RHI::DX11::DX11Texture2D::CreateRenderTarget(device_, sceneW_, sceneH_);
    sceneRT_[1] = RHI::DX11::DX11Texture2D::CreateRenderTarget(device_, sceneW_, sceneH_);
    sceneDepth_ = RHI::DX11::DX11DepthBuffer::Create(device_, sceneW_, sceneH_);

    sceneIdx_   = 0;
    sceneValid_ = false;
}

void DX11Renderer::InitDebugLayer() {
    if (debugLogger_.Initialize(device_.GetDevice())) {
        // Filter out INFO messages to reduce noise
        debugLogger_.FilterOutInfo(true);
    }
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

    sceneRT_[0] = RHI::DX11::DX11Texture2D::CreateRenderTarget(device_, sceneW_, sceneH_);
    sceneRT_[1] = RHI::DX11::DX11Texture2D::CreateRenderTarget(device_, sceneW_, sceneH_);
    sceneDepth_ = RHI::DX11::DX11DepthBuffer::Create(device_, sceneW_, sceneH_);

    sceneIdx_   = 0;
    sceneValid_ = false;
}

// ========================== Begin of Render Functions ==========================

void DX11Renderer::Present(bool vsync) {
    FlushDebugMessages();
    swapChain_.Present(vsync);
}

void DX11Renderer::FlushDebugMessages() {
    debugLogger_.FlushMessages();
}

void DX11Renderer::ExecutePlan(const RenderPlan& plan, const FrameBlackboard& frame) {
    auto& currRT = sceneRT_[sceneIdx_];
    auto& prevRT = sceneRT_[sceneIdx_ ^ 1];

    PassContext ctx{
        .device  = device_,
        .ctx = device_.GetContext(),

        .sceneRTV = currRT.RTV(),
        .sceneSRV = currRT.SRV(),
        .sceneDSV = sceneDepth_.DSV(),
        .backRTV  = swapChain_.backBufferRTV(),

        .prevSceneSRV = sceneValid_ ? prevRT.SRV() : currRT.SRV(),

        .canvasW = canvasW_,
        .canvasH = canvasH_,
        .sceneW  = sceneW_,
        .sceneH  = sceneH_,

        .states = states_,
        .cache  = stateCache_,

        .pipelines = &pipelines_,
        .draw      = &draw_,

        .frame = &frame,
    };

    for (const auto& pass : plan.passes) {
        pass->Record(ctx);
    }

    if (!frame.lockPrevScene) sceneIdx_ ^= 1;
    sceneValid_ = true;
}

// ========================== End of Render Functions ==========================

} // namespace Salt2D::Render
