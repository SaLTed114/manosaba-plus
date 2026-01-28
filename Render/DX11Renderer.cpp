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

void DX11Renderer::ExecutePlan(const RenderPlan& plan) {
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
        .cubeDemo       = &cubeDemo_
    };

    for (const auto& pass : plan.passes) {
        // Bind targets
        if (pass.target == Target::Scene) {
            ID3D11RenderTargetView* rtvs[] = { passCtx.sceneRTV };
            ctx->OMSetRenderTargets(1, rtvs, passCtx.sceneDSV);
            auto vp = MakeViewport(passCtx.sceneW, passCtx.sceneH);
            ctx->RSSetViewports(1, &vp);
        } else /*if (pass.target == Target::BackBuffer)*/ {
            ID3D11RenderTargetView* rtvs[] = { passCtx.backRTV };
            ctx->OMSetRenderTargets(1, rtvs, nullptr);
            auto vp = MakeViewport(passCtx.canvasW, passCtx.canvasH);
            ctx->RSSetViewports(1, &vp);
        }

        // Clear targets
        if (pass.clearColor) {
            auto rtv = (pass.target == Target::Scene) ? passCtx.sceneRTV : passCtx.backRTV;
            ctx->ClearRenderTargetView(rtv, pass.clearColorValue);
        }
        if (pass.clearDepth && passCtx.sceneDSV && pass.target == Target::Scene) {
            ctx->ClearDepthStencilView(passCtx.sceneDSV,
                D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
                pass.clearDepthValue, pass.clearStencilValue);
        }

        passCtx.cache.SetDepthStencilState(ctx,
            DepthStencilState(&passCtx.states, pass.depth));
        passCtx.cache.SetBlendState(ctx,
            BlendState(&passCtx.states, pass.blend));

        if (pass.exec) pass.exec(passCtx);
    }
}

// ========================== End of Render Functions ==========================

} // namespace Salt2D::Render
