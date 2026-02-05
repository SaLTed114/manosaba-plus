// App/Scene/GameScene.cpp
#include "GameScene.h"

#include "Render/Passes/SceneSpritePass.h"
#include "Render/Passes/ComposePass.h"

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

    {
        uint8_t whitePixel[4] = { 255, 255, 255, 255 };
        white1x1_ = RHI::DX11::DX11Texture2D::CreateRGBA8(device, 1, 1, whitePixel, 4);
    }

    auto graphPath = std::filesystem::path("Assets/Story/Demo/demo_story.graph.json");
    session_.Initialize(graphPath, "n0_intro");
    screens_.SetPlayer(&session_.Player());
}

// ========================= End of Initialization ==========================


// ========================= Begin of Update Functions ==========================

void GameScene::Update(
    const RHI::DX11::DX11Device& device,
    const Core::FrameTime& /*ft*/,
    const Core::InputState& in,
    uint32_t canvasW, uint32_t canvasH
) {
    screens_.Tick(in, canvasW, canvasH);
    screens_.Bake(device, text_);
}

// ========================= End of Update Functions ==========================


// ======================== Begin of Render Functions ==========================

void GameScene::FillFrameBlackboard(Render::FrameBlackboard& frame, uint32_t /*sceneW*/, uint32_t /*sceneH*/) {
    // VN MVP doesn't need a 3D camera; keep identity.
    frame.view     = XMMatrixIdentity();
    frame.proj     = XMMatrixIdentity();
    frame.viewProj = XMMatrixIdentity();
}

void GameScene::BuildDrawList(Render::DrawList& drawList, uint32_t canvasW, uint32_t canvasH) {
    (void)canvasW; (void)canvasH;

    screens_.EmitDraw(drawList, white1x1_.SRV());
}

void GameScene::BuildPlan(Render::RenderPlan& plan, const Render::DrawList& drawList) {
    using namespace Render;

    auto bg  = drawList.Sprites(Layer::Background); // probably empty
    auto hud = drawList.Sprites(Layer::HUD);

    // Clear scene target (keep your standard pipeline shape: SceneRT -> Compose -> HUD)
    auto p0 = std::make_unique<SpritePass>("Scene_BG_2D", Target::Scene, DepthMode::Off, BlendMode::Alpha, bg);
    p0->SetClearScene(0.15f, 0.15f, 0.18f, 1.0f);
    plan.passes.push_back(std::move(p0));

    // Compose scene to backbuffer
    auto p1 = std::make_unique<ComposePass>("Compose");
    plan.passes.push_back(std::move(p1));

    // HUD
    auto p2 = std::make_unique<SpritePass>("HUD_2D", Target::BackBuffer, DepthMode::Off, BlendMode::Alpha, hud);
    plan.passes.push_back(std::move(p2));
}

// ======================== End of Render Functions ==========================
    
} // namespace Salt2D::App
