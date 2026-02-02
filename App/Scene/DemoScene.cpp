// App/Scene/DemoScene.cpp
#include "DemoScene.h"
#include "Resources/Image/WICImageLoader.h"
#include "Resources/Mesh/MeshLoader.h"
#include "Render/Scene3D/MeshFactory.h"
#include "Render/Text/TextBaker.h"
#include "Render/Passes/SceneSpritePass.h"
#include "Render/Passes/ComposePass.h"
#include "Render/Passes/MeshPass.h"
#include "Render/Passes/CardPass.h"
#include "Render/Draw/MeshDrawItem.h"
#include "Render/Pipelines/MeshPipeline.h"

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

    Resources::ImageRGBA8 img1;
    std::filesystem::path img1Path = "Assets/Image/667.png";
    if (!Resources::LoadImageRGBA8_WIC(img1Path.string(), img1)) {
        throw std::runtime_error("Failed to load image in DemoScene: " + img1Path.string());
    }
    img1_ = RHI::DX11::DX11Texture2D::CreateRGBA8(device, img1.width, img1.height, img1.pixels.data(), img1.rowPitch);

    Resources::ImageRGBA8 img2;
    std::filesystem::path img2Path = "Assets/Image/659.png";
    if (!Resources::LoadImageRGBA8_WIC(img2Path.string(), img2)) {
        throw std::runtime_error("Failed to load image in DemoScene: " + img2Path.string());
    }
    img2_ = RHI::DX11::DX11Texture2D::CreateRGBA8(device, img2.width, img2.height, img2.pixels.data(), img2.rowPitch);

    Resources::ImageRGBA8 platformImg;
    std::filesystem::path platformPath = "Assets/Image/platform.png";
    if (!Resources::LoadImageRGBA8_WIC(platformPath.string(), platformImg)) {
        throw std::runtime_error("Failed to load image in DemoScene: " + platformPath.string());
    }
    platform_ = RHI::DX11::DX11Texture2D::CreateRGBA8(
        device, platformImg.width, platformImg.height,
        platformImg.pixels.data(), platformImg.rowPitch);

    // Load cube mesh from OBJ file
    // auto meshData = Resources::MeshLoader::LoadOBJ("Assets/Mesh/ring13.obj");
    // if (!meshData) throw std::runtime_error("Failed to load ring13.obj");

    camera_.SetPosition(0.0f, 1.4f, 0.0f);
    camera_.SetFovY(60.0f * DirectX::XM_PI / 180.0f);

    style_.fontFamily = L"SimSun";
    style_.fontSize = 50.0f;
    style_.weight = DWRITE_FONT_WEIGHT_HEAVY;
    textDirty_ = true;

    // Create 13 cards in a circle around origin, all facing center
    cardItems_.clear();
    cardItems_.reserve(26); // 13 cards + 13 platforms

    const int numCards = 13;
    const float radius = 3.0f;
    
    for (int i = 0; i < numCards; ++i) {
        float angle = (float)i * (2.0f * DirectX::XM_PI / numCards);
        
        Render::CardDrawItem card{};
        card.srv = img2_.SRV();
        card.size = DirectX::XMFLOAT2(1.0f, 1.9f);
        card.pos = DirectX::XMFLOAT3(radius * cosf(angle), 0.0f, radius * sinf(angle));
        card.yaw = -(angle + DirectX::XM_PIDIV2);  // -(angle + π/2)
        
        cardItems_.push_back(card);

        Render::CardDrawItem platformCard{};
        platformCard.srv = platform_.SRV();
        platformCard.size = DirectX::XMFLOAT2(1.5f, 1.0f);
        platformCard.pos = DirectX::XMFLOAT3((radius - 0.5f) * cosf(angle), 0.0f, (radius - 0.5f) * sinf(angle));
        platformCard.yaw = -(angle + DirectX::XM_PIDIV2);  // -(angle + π/2)
        
        cardItems_.push_back(platformCard);
    }
}

void DemoScene::Update(const RHI::DX11::DX11Device& device, const Core::FrameTime& ft, const Core::InputState& in, uint32_t /*canvasW*/, uint32_t /*canvasH*/) {
    float t = static_cast<float>(ft.totalSec);
    angle_ += static_cast<float>(ft.dtSec) * 0.2f;
    camera_.SetYawPitchRoll(angle_, sinf(t) * 0.02f, cos(t) * 0.01f);

    if (!textInited_) { textBaker_.Initialize(); textInited_ = true; }

    if (in.Pressed(Core::Key::LButton)) {
        text_ = L"Mouse Clicked at (" + std::to_wstring(in.mouseX) + L", " + std::to_wstring(in.mouseY) + L")";
        textDirty_ = true;
    }
    if (in.Released(Core::Key::LButton)) {
        text_ = L"Mouse Released at (" + std::to_wstring(in.mouseX) + L", " + std::to_wstring(in.mouseY) + L")";
        textDirty_ = true;
    }

    if (!textDirty_) return;

    textDirty_ = false;
    bakedText_ = textBaker_.BakeToTexture(
        device, text_, style_, layoutW_, layoutH_);
}

void DemoScene::FillFrameBlackboard(Render::FrameBlackboard& frame, uint32_t sceneW, uint32_t sceneH) {
    frame.view = camera_.GetView();
    frame.proj = camera_.GetProj(sceneW, sceneH);
    frame.viewProj = frame.view * frame.proj;
}

void DemoScene::BuildDrawList(Render::DrawList& drawList, uint32_t canvasW, uint32_t canvasH) {
    drawList.ReserveSprites(8);

    drawList.PushSprite(Render::Layer::Background, checker_.SRV(),
        Render::RectF{0,0,static_cast<float>(canvasW),static_cast<float>(canvasH)}, 0.0f);

    auto& s = drawList.PushSprite(Render::Layer::Stage, img1_.SRV(),
        Render::RectF{100,500,static_cast<float>(img1_.GetWidth()/4),static_cast<float>(img1_.GetHeight())/4}, 0.0f);

    s.clipEnabled = true;
    s.clipRect = Render::RectI{150,550,300,700};

    drawList.PushSprite(Render::Layer::HUD, checker_.SRV(),
        Render::RectF{20,20,256,256}, 0.0f);

    drawList.PushSprite(Render::Layer::Text, bakedText_.tex.SRV(), 
        Render::RectF{50,300,static_cast<float>(bakedText_.w),static_cast<float>(bakedText_.h)}, 0.0f);
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

    // 3D cubes - depth testing with partial occlusion
    meshItems_.clear();
    auto p1 = std::make_unique<Render::MeshPass>("Scene_3D", Target::Scene, DepthMode::RW, BlendMode::Off, meshItems_);
    plan.passes.push_back(std::move(p1));

    auto pCard = std::make_unique<CardPass>("Scene_3D_Card", Target::Scene, DepthMode::RW, BlendMode::Alpha, cardItems_);
    plan.passes.push_back(std::move(pCard));

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
