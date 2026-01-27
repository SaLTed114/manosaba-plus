// Render/DX11Renderer.cpp
#include "DX11Renderer.h"
#include "Utils/FileUtils.h"
#include "Resources/Image/WICImageLoader.h"

#include "DX11RenderUtils.h"
#include "Render/Primitives/TestPattern.h"

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
    
    InitDemoTextures();

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

RenderPlan DX11Renderer::BuildDefaultPlan() {
    RenderPlan plan;

    auto bg      = drawList_.Sprites(Layer::Background);
    auto overlay = drawList_.Sprites(Layer::Stage, Layer::Text);
    auto hud     = drawList_.Sprites(Layer::HUD);

    {
        RenderPass pass;
        pass.name = "Scene_BG_2D";
        pass.target = Target::Scene;
        pass.clearColor = true;
        pass.clearDepth = true;
        pass.clearColorValue[0] = 0.2f; pass.clearColorValue[1] = 0.2f;
        pass.clearColorValue[2] = 0.2f; pass.clearColorValue[3] = 1.0f;
        pass.depth = DepthMode::Off;
        pass.blend = BlendMode::Alpha;
        pass.exec = [this, bg](PassContext& ctx) {
            spriteRenderer_.Draw(ctx.device, bg, ctx.canvasW, ctx.canvasH);
        };
        plan.passes.push_back(std::move(pass));
    }

    {
        RenderPass pass;
        pass.name = "Scene_3D_Cube";
        pass.target = Target::Scene;
        pass.depth = DepthMode::RW;
        pass.blend = BlendMode::Off;
        pass.exec = [this](PassContext& ctx) {
            static float angle = 0.0f; angle += 0.01f;
            using namespace DirectX;
            XMMATRIX world = XMMatrixRotationY(angle) * XMMatrixRotationX(angle * 0.5f);
            XMVECTOR eye  = XMVectorSet(0.0f, 1.2f, -2.5f, 1.0f);
            XMVECTOR at   = XMVectorZero();
            XMVECTOR up   = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
            XMMATRIX view = XMMatrixLookAtLH(eye, at, up);
            float aspect  = static_cast<float>(ctx.sceneW) / static_cast<float>(ctx.sceneH);
            XMMATRIX proj = XMMatrixPerspectiveFovLH(XM_PIDIV4, aspect, 0.1f, 100.0f);

            cubeDemo_.Draw(ctx.device, world * view * proj);
        };
        plan.passes.push_back(std::move(pass));
    }

    {
        RenderPass pass;
        pass.name = "Scene_Overlay_2D";
        pass.target = Target::Scene;
        pass.depth = DepthMode::Off;
        pass.blend = BlendMode::Alpha;
        pass.exec = [this, overlay](PassContext& ctx) {
            spriteRenderer_.Draw(ctx.device, overlay, ctx.canvasW, ctx.canvasH);
        };
        plan.passes.push_back(std::move(pass));
    }

    {
        RenderPass pass;
        pass.name = "Compose";
        pass.target = Target::BackBuffer;
        pass.depth = DepthMode::Off;
        pass.blend = BlendMode::Off;
        pass.exec = [this](PassContext& ctx) {
            compose_.Draw(ctx.device, ctx.sceneSRV);
        };
        plan.passes.push_back(std::move(pass));
    }

    {
        RenderPass pass;
        pass.name = "HUD_2D";
        pass.target = Target::BackBuffer;
        pass.depth = DepthMode::Off;
        pass.blend = BlendMode::Alpha;
        pass.exec = [this, hud](PassContext& ctx) {
            spriteRenderer_.Draw(ctx.device, hud, ctx.canvasW, ctx.canvasH);
        };
        plan.passes.push_back(std::move(pass));
    }

    return plan;
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

void DX11Renderer::BuildTestDrawList() {
    drawList_.Clear();
    drawList_.ReserveSprites(8);

    auto srv = testTexture_.SRV();
    drawList_.PushSprite(Layer::Background, srv, RectF{0,0,(float)canvasW_,(float)canvasH_}, 0.0f);
    drawList_.PushSprite(Layer::HUD, srv, RectF{20,20,256,256}, 0.0f);

    auto imgSrv = imgTexture_.SRV();
    drawList_.PushSprite(Layer::Stage, imgSrv, RectF{100,100, (float)imgTexture_.GetWidth(), (float)imgTexture_.GetHeight()}, 0.0f);

    drawList_.Sort();
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
        .cache  = stateCache_
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


// ========================== Begin of Main Render Function ==========================

void DX11Renderer::RenderFrame(bool vsync) {
    if (canvasW_ == 0 || canvasH_ == 0) return;
    if (sceneW_  == 0 || sceneH_  == 0) return;

    BuildTestDrawList(); // TMP

    auto plan = BuildDefaultPlan();
    ExecutePlan(plan);

    Present(vsync);
}

// ========================== End of Main Render Function ==========================

} // namespace Salt2D::Render
