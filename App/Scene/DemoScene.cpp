// App/Scene/DemoScene.cpp
#include "DemoScene.h"
#include "Resources/Image/WICImageLoader.h"
#include "Render/Passes/SceneSpritePass.h"
#include "Render/Passes/ComposePass.h"
#include "Render/Passes/CubePass.h"

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
    float t = static_cast<float>(ft.totalSec);
    angle_ += static_cast<float>(ft.dtSec) * 1.0f;
    float deg2rad = DirectX::XM_PI / 180.0f;
    camera_.SetYawPitchRoll(sinf(t) * 30.0f * deg2rad, sinf(t*2) * 20.0f * deg2rad, 0.0f);
}

void DemoScene::FillFrameBlackboard(Render::FrameBlackboard& frame, uint32_t sceneW, uint32_t sceneH) {
    frame.view = camera_.GetView();
    frame.proj = camera_.GetProj(sceneW, sceneH);
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

    // Scene background
    auto p0 = std::make_unique<SpritePass>("Scene_BG_2D", Target::Scene, DepthMode::Off, BlendMode::Alpha, bg);
    p0->SetClearScene(0.2f, 0.2f, 0.2f, 1.0f);
    plan.passes.push_back(std::move(p0));

    // 3D cube
    XMMATRIX world = XMMatrixRotationY(angle_) * XMMatrixRotationX(angle_ * 0.5f);
    auto p1 = std::make_unique<CubePass>("Scene_3D_Cube", world);
    plan.passes.push_back(std::move(p1));

    // Scene overlay sprites
    auto p2 = std::make_unique<SpritePass>("Scene_Overlay_2D", Target::Scene, DepthMode::Off, BlendMode::Alpha, overlay);
    plan.passes.push_back(std::move(p2));

    // Compose scene to backbuffer
    auto p3 = std::make_unique<ComposePass>("Compose");
    plan.passes.push_back(std::move(p3));

    // HUD sprites
    auto p4 = std::make_unique<SpritePass>("HUD_2D", Target::BackBuffer, DepthMode::Off, BlendMode::Alpha, hud);
    plan.passes.push_back(std::move(p4));
}

} // namespace Salt2D::App
