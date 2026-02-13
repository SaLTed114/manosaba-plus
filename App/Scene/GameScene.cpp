// App/Scene/GameScene.cpp
#include "GameScene.h"

#include "Render/Passes/SceneSpritePass.h"
#include "Render/Passes/CardPass.h"
#include "Render/Passes/ComposePass.h"
#include "Resources/Image/WICImageLoader.h"

#include <iostream>
#include <Windows.h>
#include <algorithm>
#include <stdexcept>

using namespace DirectX;

namespace Salt2D::App {

// ========================= Begin of Initialization ==========================

GameScene::GameScene(Utils::DiskFileSystem& fs) : fs_(fs), session_(fs) {}

void GameScene::Initialize(Render::DX11Renderer& renderer) {
    auto device = renderer.Device();

    text_.Initialize();
    text_.ClearCache();

    texService_.Clear();
    texCatalog_.Clear();

    {
        uint8_t whitePixel[4] = { 255, 255, 255, 255 };
        white1x1_ = RHI::DX11::DX11Texture2D::CreateRGBA8(device, 1, 1, whitePixel, 4);
    }

    // purple-black checkerboard texture placeholder
    {
        const uint32_t size = 64;
        const uint32_t cell = 8;
        std::vector<uint8_t> checkerData(size * size * 4);

        auto setPx = [&](uint32_t x, uint32_t y, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
            uint32_t idx = (y * size + x) * 4;
            checkerData[idx + 0] = r;
            checkerData[idx + 1] = g;
            checkerData[idx + 2] = b;
            checkerData[idx + 3] = a;
        };

        const uint8_t pR=120, pG= 0, pB=120, pA=255;
        const uint8_t bR= 15, bG=15, bB= 18, bA=255;

        for (uint32_t y = 0; y < size; ++y) {
            for (uint32_t x = 0; x < size; ++x) {
                bool isPurple = ((x / cell) ^ (y / cell)) & 1;
                if (isPurple) setPx(x, y, pR, pG, pB, pA);
                else setPx(x, y, bR, bG, bB, bA);
            }
        }
        checker_ = RHI::DX11::DX11Texture2D::CreateRGBA8(device, size, size, checkerData.data(), size * 4);
    }

    texService_.Register(Game::UI::TextureId::White, white1x1_.SRV(), 1, 1);
    texService_.Register(Game::UI::TextureId::Checker, checker_.SRV(), 64, 64);

    texService_.SetMissing(Game::UI::TextureId::Checker);

    texCatalog_.SetAssetsRoot("Assets/");
    texCatalog_.SetMissing({ checker_.SRV(), 64, 64, true });
    // TODO: catalog settings like missing texture and logger

    auto storyRoot = std::filesystem::path("Assets/Story/Demo/");
    auto graphPath = std::filesystem::path("demo_story.graph.json");
    session_.Initialize(storyRoot, graphPath, "n0_intro");
    screens_.Initialize(&session_.Player(), &session_.History(), &session_.Tables());

    stage_.Initialize(&session_.Tables(), &texCatalog_);
    stage_.LoadStage(device, "inquisition");

    camera_.SetPosition({0.0f, 1.4f, 0.0f});
    camera_.SetFovY(60.0f * XM_PI / 180.0f);
    camera_.SetClip(0.1f, 100.0f);

    director_.Initialize(&stage_, &session_.Tables(), &camera_);
}

// ========================= End of Initialization ==========================


// ========================= Begin of Update Functions ==========================

void GameScene::Update(
    const RHI::DX11::DX11Device& device,
    const Core::FrameTime& ft,
    const Core::InputState& in,
    uint32_t canvasW, uint32_t canvasH
) {
    screens_.Tick(ft, in, canvasW, canvasH);
    screens_.Bake(device, text_);
    screens_.PostBake(in, canvasW, canvasH);

    director_.Tick(session_.Player(), ft);
}

// ========================= End of Update Functions ==========================


// ======================== Begin of Render Functions ==========================

void GameScene::FillFrameBlackboard(Render::FrameBlackboard& frame, uint32_t sceneW, uint32_t sceneH) {
    frame.view     = camera_.GetView();
    frame.proj     = camera_.GetProj(sceneW, sceneH);
    frame.viewProj = XMMatrixMultiply(frame.view, frame.proj);
}

void GameScene::BuildDrawList(Render::DrawList& drawList, uint32_t canvasW, uint32_t canvasH) {
    (void)canvasW; (void)canvasH;

    stage_.EmitBackground(drawList, canvasW, canvasH);
    screens_.EmitDraw(drawList, texService_);

}

void GameScene::BuildPlan(Render::RenderPlan& plan, const Render::DrawList& drawList) {
    using namespace Render;

    auto bg  = drawList.Sprites(Layer::Background); // probably empty
    auto hud = drawList.Sprites(Layer::HUD);

    // Clear scene target (keep your standard pipeline shape: SceneRT -> Compose -> HUD)
    auto p0 = std::make_unique<SpritePass>("Scene_BG_2D", Target::Scene, DepthMode::Off, BlendMode::Alpha, bg);
    p0->SetClearScene(0.15f, 0.15f, 0.18f, 1.0f);
    plan.passes.push_back(std::move(p0));

    auto pCard = std::make_unique<CardPass>("Scene_Cards_2D", Target::Scene, DepthMode::RW, BlendMode::Alpha, stage_.Cards());
    plan.passes.push_back(std::move(pCard));

    // Compose scene to backbuffer
    auto p1 = std::make_unique<ComposePass>("Compose");
    plan.passes.push_back(std::move(p1));

    // HUD
    auto p2 = std::make_unique<SpritePass>("HUD_2D", Target::BackBuffer, DepthMode::Off, BlendMode::Alpha, hud);
    plan.passes.push_back(std::move(p2));
}

// ======================== End of Render Functions ==========================
    
} // namespace Salt2D::App
