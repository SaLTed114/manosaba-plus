// App/Scene/DemoScene.cpp
#include "DemoScene.h"
#include "Resources/Image/WICImageLoader.h"

using namespace DirectX;

namespace Salt2D::App {

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

} // Anonymous namespace

void DemoScene::Initialize(Render::DX11Renderer& renderer) {
    auto& device = renderer.Device();

    auto rgba = MakeCheckerRGBA(256, 256);
    checker_ = RHI::DX11::DX11Texture2D::CreateRGBA8(device, 256, 256, rgba.data(), 256 * 4);

    Resources::ImageRGBA8 img;
    std::filesystem::path imgPath = "Assets/Textures/667.png";
    if (!Resources::LoadImageRGBA8_WIC(imgPath.string(), img)) {
        throw std::runtime_error("Failed to load image in DemoScene: " + imgPath.string());
    }
    img_ = RHI::DX11::DX11Texture2D::CreateRGBA8(device, img.width, img.height, img.pixels.data(), img.rowPitch);
}

void DemoScene::Update(const Core::FrameTime& ft, uint32_t /*canvasW*/, uint32_t /*canvasH*/) {
    angle_ += static_cast<float>(ft.dtSec) * 1.0f;
}

void DemoScene::BuildDrawList(Render::DrawList& drawList, uint32_t canvasW, uint32_t canvasH) {
    drawList.ReserveSprites(8);

    drawList.PushSprite(Render::Layer::Background, checker_.SRV(),
        Render::RectF{0,0,static_cast<float>(canvasW),static_cast<float>(canvasH)}, 0.0f);

    drawList.PushSprite(Render::Layer::Stage, img_.SRV(),
        Render::RectF{100,100,static_cast<float>(img_.GetWidth()),static_cast<float>(img_.GetHeight())}, 0.0f);

    drawList.PushSprite(Render::Layer::HUD, checker_.SRV(),
        Render::RectF{20,20,256,256}, 0.0f);
}

void DemoScene::BuildPlan(Render::RenderPlan& plan, const Render::DrawList& drawList) {
    using namespace Render;

    auto bg      = drawList.Sprites(Layer::Background);
    auto overlay = drawList.Sprites(Layer::Stage, Layer::Text);
    auto hud     = drawList.Sprites(Layer::HUD);

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
            ctx.spriteRenderer->Draw(ctx.device, bg, ctx.canvasW, ctx.canvasH);
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
            using namespace DirectX;
            XMMATRIX world = XMMatrixRotationY(angle_) * XMMatrixRotationX(angle_ * 0.5f);
            XMVECTOR eye  = XMVectorSet(0.0f, 1.2f, -2.5f, 1.0f);
            XMVECTOR at   = XMVectorZero();
            XMVECTOR up   = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
            XMMATRIX view = XMMatrixLookAtLH(eye, at, up);
            float aspect  = static_cast<float>(ctx.sceneW) / static_cast<float>(ctx.sceneH);
            XMMATRIX proj = XMMatrixPerspectiveFovLH(XM_PIDIV4, aspect, 0.1f, 100.0f);

            ctx.cubeDemo->Draw(ctx.device, world * view * proj);
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
            ctx.spriteRenderer->Draw(ctx.device, overlay, ctx.canvasW, ctx.canvasH);
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
            ctx.compose->Draw(ctx.device, ctx.sceneSRV);
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
            ctx.spriteRenderer->Draw(ctx.device, hud, ctx.canvasW, ctx.canvasH);
        };
        plan.passes.push_back(std::move(pass));
    }
}

} // namespace Salt2D::App
