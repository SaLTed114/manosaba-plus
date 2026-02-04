// App/Scene/GameScene.cpp
#include "GameScene.h"

#include "Game/Story/StoryGraphLoader.h"
#include "Render/Passes/SceneSpritePass.h"
#include "Render/Passes/ComposePass.h"
#include "Utils/FileUtils.h"

#include <Windows.h>
#include <algorithm>
#include <stdexcept>

using namespace DirectX;

namespace Salt2D::App {

GameScene::GameScene(Utils::DiskFileSystem& fs) : fs_(fs) {}

std::wstring GameScene::Utf8ToWString(const std::string& str) {
    if (str.empty()) return L"";
    int size_needed = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), (int)str.size(), NULL, 0);
    if (size_needed <= 0) return L"";
    std::wstring wstrTo(size_needed, 0);
    MultiByteToWideChar(CP_UTF8, 0, str.c_str(), (int)str.size(), &wstrTo[0], size_needed);
    return wstrTo;
}

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
        vnTextDirty_ = true;
    }
}

void GameScene::UpdateUILayout(uint32_t canvasW, uint32_t canvasH) {
    if (canvasW == lastCanvasW_ && canvasH == lastCanvasH_) return;

    lastCanvasW_ = canvasW;
    lastCanvasH_ = canvasH;

    const float margin = 24.0f;
    const float boxH = (std::min)(260.0f, canvasH * 0.3f);

    vnBox_.x = margin;
    vnBox_.y = static_cast<float>(canvasH - margin - boxH);
    vnBox_.w = static_cast<float>(canvasW - margin * 2.0f);
    vnBox_.h = boxH;

    const float innerW = (std::max)(1.0f, vnBox_.w - vnPad_ * 2.0f);
    const float innerH = (std::max)(1.0f, vnBox_.h - vnPad_ * 2.0f);

    speakerLayoutW_ = innerW;
    speakerLayoutH_ = 44.0f;

    bodyLayoutW_ = innerW;
    bodyLayoutH_ = (std::max)(1.0f, innerH - speakerLayoutH_ - 8.0f);

    vnTextDirty_ = true;
}

void GameScene::BakeVNIfDirty(const RHI::DX11::DX11Device& device) {
    if (!player_) return;

    const auto& view = player_->View();
    if (!view.vn.has_value()) return;

    const auto& vn = view.vn.value();
    const std::string& speaker = vn.speaker;
    const std::string& body = vn.fullText;

    if (!vnTextDirty_ && speaker == lastSpeaker_ && body == lastBody_) return;

    lastSpeaker_ = speaker;
    lastBody_ = body;
    vnTextDirty_ = false;

    speakerTex_ = textBaker_.BakeToTexture(
        device, Utf8ToWString(speaker), speakerStyle_,
        speakerLayoutW_, speakerLayoutH_);

    bodyTex_ = textBaker_.BakeToTexture(
        device, Utf8ToWString(body), bodyStyle_,
        bodyLayoutW_, bodyLayoutH_);
}

void GameScene::Update(
    const RHI::DX11::DX11Device& device,
    const Core::FrameTime& /*ft*/,
    const Core::InputState& in,
    uint32_t canvasW, uint32_t canvasH
) {
    if (!player_) return;

    UpdateUILayout(canvasW, canvasH);

    const bool advancePressed = in.Pressed(VK_SPACE) || in.Pressed(VK_RETURN);
    if (advancePressed) {
        if (in.Down(VK_SHIFT)) player_->FastForward();
        else player_->Advance();
        vnTextDirty_ = true;
    }

    BakeVNIfDirty(device);
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

    const auto& view = player_->View();
    if (!view.vn.has_value()) return;

    // VN panel background
    {
        Render::Color4F panelTint{0.0f, 0.0f, 0.0f, 0.55f};
        drawList.PushSprite(Render::Layer::HUD, white1x1_.SRV(), vnBox_, 0.0f, {}, panelTint);
    }

    const float x = vnBox_.x + vnPad_;
    float y = vnBox_.y + vnPad_;

    // Speaker
    if (speakerTex_.tex.SRV()) {
        Render::RectF dst{
            x, y,
            (float)speakerTex_.w,
            (float)speakerTex_.h
        };
        drawList.PushSprite(Render::Layer::HUD, speakerTex_.tex.SRV(), dst);
        y += (float)speakerTex_.h + 8.0f;
    }

    // Body
    if (bodyTex_.tex.SRV()) {
        Render::RectF dst{
            x, y,
            (float)bodyTex_.w,
            (float)bodyTex_.h
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
