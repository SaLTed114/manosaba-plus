// App/Scene/GameScene.cpp
#include "GameScene.h"

#include "Game/Story/StoryGraphLoader.h"
#include "Game/UI/VnHud.h"
#include "Render/Passes/SceneSpritePass.h"
#include "Render/Passes/ComposePass.h"
#include "Utils/FileUtils.h"

#include <Windows.h>
#include <algorithm>
#include <stdexcept>

using namespace DirectX;

namespace Salt2D::App {

GameScene::GameScene(Utils::DiskFileSystem& fs) : fs_(fs) {}

void GameScene::EnsureTextSystem() {
    if (textInited_) return;

    textBaker_.Initialize();
    textInited_ = true;

    speakerStyle_.fontFamily = L"SimSun";
    speakerStyle_.fontSize = 24.0f;
    speakerStyle_.weight = DWRITE_FONT_WEIGHT_BOLD;

    bodyStyle_.fontFamily = L"SimSun";
    bodyStyle_.fontSize = 20.0f;
    bodyStyle_.weight = DWRITE_FONT_WEIGHT_REGULAR;
}

void GameScene::Initialize(Render::DX11Renderer& renderer) {
    auto device = renderer.Device();

    logger_ = Game::MakeConsoleAndFileLogger(
        Utils::GenerateTimestampedFilename("Logs/game_scene.log"),
        Game::LogLevel::Debug, Game::LogLevel::Debug);

    textCache_.Clear();

    EnsureTextSystem();

    {
        uint8_t whitePixel[4] = { 255, 255, 255, 255 };
        white1x1_ = RHI::DX11::DX11Texture2D::CreateRGBA8(device, 1, 1, whitePixel, 4);
    }

    {
        auto graphPath = std::filesystem::path("Assets/Story/Demo/demo_story.graph.json");
        graph_ = Game::Story::LoadStoryGraph(fs_, graphPath);

        player_ = std::make_unique<Game::Story::StoryPlayer>(graph_, fs_);
        player_->SetEffectCallback([](const Salt2D::Game::Story::Effect& e) {
            // Placeholder: later route to Director/CameraRig/etc.
            OutputDebugStringA(("[Effect] type=" + e.type + " name=" + e.name + "\n").c_str());
        });

        player_->Start("n0_intro");
        player_->SetLogger(&logger_);
    }
}

void GameScene::Update(
    const RHI::DX11::DX11Device& device,
    const Core::FrameTime& /*ft*/,
    const Core::InputState& in,
    uint32_t canvasW, uint32_t canvasH
) {
    if (!player_) return;

    const bool advancePressed = in.Pressed(VK_SPACE) || in.Pressed(VK_RETURN);
    if (advancePressed) {
        if (in.Down(VK_SHIFT)) player_->FastForward();
        else player_->Advance();
    }

    Game::UI::VnHudModel vnModel;
    const auto& vnView = player_->View().vn;
    if (vnView.has_value()) {
        vnModel.visible = true;
        vnModel.speakerUtf8 = vnView->speaker;
        vnModel.bodyUtf8 = vnView->fullText;
    } else {
        vnModel.visible = false;
    }

    vnDraw_ = vnHud_.Build(vnModel, canvasW, canvasH);
    if (!vnDraw_.visible) return;

    speakerTex_ = textCache_.GetOrBake(
        device, textBaker_,
        static_cast<uint8_t>(Game::UI::TextStyleId::Speaker),
        speakerStyle_, vnModel.speakerUtf8,
        vnDraw_.speaker.layoutW, vnDraw_.speaker.layoutH);

    bodyTex_ = textCache_.GetOrBake(
        device, textBaker_,
        static_cast<uint8_t>(Game::UI::TextStyleId::Body),
        bodyStyle_, vnModel.bodyUtf8,
        vnDraw_.body.layoutW, vnDraw_.body.layoutH);
}

void GameScene::FillFrameBlackboard(Render::FrameBlackboard& frame, uint32_t /*sceneW*/, uint32_t /*sceneH*/) {
    // VN MVP doesn't need a 3D camera; keep identity.
    frame.view     = XMMatrixIdentity();
    frame.proj     = XMMatrixIdentity();
    frame.viewProj = XMMatrixIdentity();
}

void GameScene::BuildDrawList(Render::DrawList& drawList, uint32_t canvasW, uint32_t canvasH) {
    (void)canvasW; (void)canvasH;

    if (!player_) return;

    if (!vnDraw_.visible) return;

    // VN panel background
    {
        drawList.PushSprite(Render::Layer::HUD, white1x1_.SRV(),
            vnDraw_.panel, 0.0f, {}, vnDraw_.panelTint);
    }

    // Speaker
    if (speakerTex_.tex.SRV()) {
        Render::RectF dst{
            vnDraw_.speaker.x, vnDraw_.speaker.y,
            static_cast<float>(speakerTex_.w),
            static_cast<float>(speakerTex_.h)
        };
        drawList.PushSprite(Render::Layer::HUD, speakerTex_.tex.SRV(), dst);
    }

    // Body
    if (bodyTex_.tex.SRV()) {
        Render::RectF dst{
            vnDraw_.body.x, vnDraw_.body.y,
            static_cast<float>(bodyTex_.w),
            static_cast<float>(bodyTex_.h)
        };
        drawList.PushSprite(Render::Layer::HUD, bodyTex_.tex.SRV(), dst);
    }
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

    
} // namespace Salt2D::App
