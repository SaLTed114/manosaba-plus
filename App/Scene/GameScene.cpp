// App/Scene/GameScene.cpp
#include "GameScene.h"

#include "Game/Story/StoryGraphLoader.h"
#include "Game/UI/VnHud.h"
#include "Render/Passes/SceneSpritePass.h"
#include "Render/Passes/ComposePass.h"
#include "Utils/FileUtils.h"

#include <iostream>
#include <Windows.h>
#include <algorithm>
#include <stdexcept>

using namespace DirectX;

namespace Salt2D::App {

namespace {

static int ClampWarp(int v, int n) {
    if (n <= 0) return 0;
    v %= n;
    if (v < 0) v += n;
    return v;
}

}

// ========================= Begin of Initialization ==========================

GameScene::GameScene(Utils::DiskFileSystem& fs) : fs_(fs) {}

void GameScene::EnsureTextSystem() {
    if (textInited_) return;

    textBaker_.Initialize();
    textInited_ = true;

    vnSpeakerStyle_.fontFamily = L"SimSun";
    vnSpeakerStyle_.fontSize = 24.0f;
    vnSpeakerStyle_.weight = DWRITE_FONT_WEIGHT_BOLD;

    vnBodyStyle_.fontFamily = L"SimSun";
    vnBodyStyle_.fontSize = 20.0f;
    vnBodyStyle_.weight = DWRITE_FONT_WEIGHT_REGULAR;

    debateSpeakerStyle_.fontFamily = L"SimSun";
    debateSpeakerStyle_.fontSize = 24.0f;
    debateSpeakerStyle_.weight = DWRITE_FONT_WEIGHT_BOLD;

    debateBodyStyle_.fontFamily = L"SimSun";
    debateBodyStyle_.fontSize = 20.0f;
    debateBodyStyle_.weight = DWRITE_FONT_WEIGHT_REGULAR;

    debateSmallStyle_.fontFamily = L"SimSun";
    debateSmallStyle_.fontSize = 16.0f;
    debateSmallStyle_.weight = DWRITE_FONT_WEIGHT_REGULAR;
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

// ========================= End of Initialization ==========================


// ========================= Begin of Helper Functions ==========================

Render::Text::BakedText GameScene::BakeRequiredText(
    const RHI::DX11::DX11Device& device,
    uint8_t styleId,
    const Render::Text::TextStyle& style,
    const Game::UI::TextRequest& req
) {
    return textCache_.GetOrBake(
        device, textBaker_,
        styleId, style,
        req.textUtf8,
        req.layoutW, req.layoutH);
}

// ======================== End of Helper Functions ==========================

// ========================= Begin of Update Functions ==========================

void GameScene::Update(
    const RHI::DX11::DX11Device& device,
    const Core::FrameTime& /*ft*/,
    const Core::InputState& in,
    uint32_t canvasW, uint32_t canvasH
) {
    if (!player_) return;

    const auto& view = player_->View();

    switch (view.nodeType) {
    case Game::Story::NodeType::VN:
    case Game::Story::NodeType::Error:
    case Game::Story::NodeType::BE:
        UpdateVN(device, in, canvasW, canvasH);
        break;
    case Game::Story::NodeType::Debate:
        UpdateDebate(device, in, canvasW, canvasH);
        break;
    case Game::Story::NodeType::Present:
        // placeholder for future Present mode
        break;
    default:
        vnDraw_.visible = false;
        debateDraw_.visible = false;
        break;
    }
}

void GameScene::UpdateVN(
    const RHI::DX11::DX11Device& device,
    const Core::InputState& in,
    uint32_t canvasW, uint32_t canvasH
) {
    const bool advancePressed = in.Pressed(VK_SPACE) || in.Pressed(VK_RETURN);
    if (advancePressed) {
        if (in.Down(VK_SHIFT)) player_->FastForward();
        else player_->Advance();
    }

    Game::UI::VnHudModel model;
    const auto& vnView = player_->View().vn;
    if (vnView.has_value()) {
        model.visible = true;
        model.speakerUtf8 = vnView->speaker;
        model.bodyUtf8 = vnView->fullText;
    } else {
        model.visible = false;
    }

    vnDraw_ = vnHud_.Build(model, canvasW, canvasH);
    if (!vnDraw_.visible) return;

    vnSpeakerText_ = BakeRequiredText(
        device, static_cast<uint8_t>(vnDraw_.speaker.style),
        vnSpeakerStyle_, vnDraw_.speaker);

    vnBodyText_ = BakeRequiredText(
        device, static_cast<uint8_t>(vnDraw_.body.style),
        vnBodyStyle_, vnDraw_.body);

    debateDraw_.visible = false;
}

void GameScene::HandleDebateInput(
    const Core::InputState& in,
    const Game::Story::StoryView::DebateView& view
) {
    if (view.menuOpen && in.Pressed(VK_ESCAPE)) {
        player_->CloseDebateMenu();
        return;
    }

    if (in.Pressed(VK_UP)) {
        if (view.menuOpen) --debateSelOption_;
        else --debateSelSpan_;
    }
    if (in.Pressed(VK_DOWN)) {
        if (view.menuOpen) ++debateSelOption_;
        else ++debateSelSpan_;
    }

    const bool confirm = in.Pressed(VK_RETURN) || in.Pressed(VK_SPACE);
    const bool openMenu = in.Pressed('M');
    if (!confirm && !openMenu) return;

    if (!view.menuOpen) {
        if (!view.spanIds.empty() && openMenu) {
            const int index = ClampWarp(debateSelSpan_, static_cast<int>(view.spanIds.size()));
            player_->OpenSuspicion(view.spanIds[index]);
        }

        if (confirm) {
            player_->Advance();
        }
    } else {
        const int index = ClampWarp(debateSelOption_, static_cast<int>(view.options.size()));
        const auto& optionId = view.options[index].first;
        player_->CommitOption(optionId);
    }
}

void GameScene::BuildDebateUI(
    uint32_t canvasW,
    uint32_t canvasH,
    const Game::Story::StoryView::DebateView& view
) {
    Game::UI::DebateHudModel model;
    model.visible = true;
    model.speakerUtf8  = view.speaker;
    model.bodyUtf8     = view.fullText;
    model.spanIds      = view.spanIds;
    model.menuOpen     = view.menuOpen;
    model.openedSpanId = view.openedSpanId;
    model.menuOptions  = view.options;

    model.selectedSpan = ClampWarp(debateSelSpan_,   static_cast<int>(view.spanIds.size()));
    model.selectedOpt  = ClampWarp(debateSelOption_, static_cast<int>(view.options.size()));

    debateDraw_ = debateHud_.Build(model, canvasW, canvasH);
}

void GameScene::BakeDebate(const RHI::DX11::DX11Device& device) {
    if (!debateDraw_.visible) return;

    debateSpeakerText_ = BakeRequiredText(
        device, static_cast<uint8_t>(debateDraw_.speaker.style),
        debateSpeakerStyle_, debateDraw_.speaker);

    debateBodyText_ = BakeRequiredText(
        device, static_cast<uint8_t>(debateDraw_.body.style),
        debateBodyStyle_, debateDraw_.body);

    debateSpanTexts_.clear();
    debateSpanTexts_.reserve(debateDraw_.spans.size());
    for (const auto& spanReq : debateDraw_.spans) {
        auto baked = BakeRequiredText(
            device, static_cast<uint8_t>(spanReq.style),
            debateBodyStyle_, spanReq);
        debateSpanTexts_.push_back(std::move(baked));
    }

    debateOptionTexts_.clear();
    debateOptionTexts_.reserve(debateDraw_.options.size());
    for (const auto& optReq : debateDraw_.options) {
        auto baked = BakeRequiredText(
            device, static_cast<uint8_t>(optReq.style),
            debateSmallStyle_, optReq);
        debateOptionTexts_.push_back(std::move(baked));
    }
}

void GameScene::UpdateDebate(
    const RHI::DX11::DX11Device& device,
    const Core::InputState& in,
    uint32_t canvasW,
    uint32_t canvasH
) {
    const auto& view = player_->View().debate;
    if (!view.has_value()) {
        debateDraw_.visible = false;
        return;
    }

    HandleDebateInput(in, view.value());

    const auto& updatedView = player_->View().debate;
    if (!updatedView.has_value()) {
        debateDraw_.visible = false;
        return;
    }
    
    BuildDebateUI(canvasW, canvasH, updatedView.value());
    BakeDebate(device);

    vnDraw_.visible = false;
}

// ========================= End of Update Functions ==========================


// ======================== Begin of Render Functions ==========================

void GameScene::FillFrameBlackboard(Render::FrameBlackboard& frame, uint32_t /*sceneW*/, uint32_t /*sceneH*/) {
    // VN MVP doesn't need a 3D camera; keep identity.
    frame.view     = XMMatrixIdentity();
    frame.proj     = XMMatrixIdentity();
    frame.viewProj = XMMatrixIdentity();
}

void GameScene::DrawVN(Render::DrawList& drawList) {
    if (!vnDraw_.visible) return;

    // VN panel background
    {
        drawList.PushSprite(Render::Layer::HUD, white1x1_.SRV(),
            vnDraw_.panel, 0.0f, {}, vnDraw_.panelTint);
    }

    // Speaker
    if (vnSpeakerText_.tex.SRV()) {
        Render::RectF dst{
            vnDraw_.speaker.x, vnDraw_.speaker.y,
            static_cast<float>(vnSpeakerText_.w),
            static_cast<float>(vnSpeakerText_.h)
        };
        drawList.PushSprite(Render::Layer::HUD, vnSpeakerText_.tex.SRV(), dst);
    }

    // Body
    if (vnBodyText_.tex.SRV()) {
        Render::RectF dst{
            vnDraw_.body.x, vnDraw_.body.y,
            static_cast<float>(vnBodyText_.w),
            static_cast<float>(vnBodyText_.h)
        };
        drawList.PushSprite(Render::Layer::HUD, vnBodyText_.tex.SRV(), dst);
    }
}

void GameScene::DrawDebate(Render::DrawList& drawList) {
    if (!debateDraw_.visible) return;

    // Debate panel background
    {
        drawList.PushSprite(Render::Layer::HUD, white1x1_.SRV(),
            debateDraw_.panel, 0.0f, {}, debateDraw_.panelTint);
    }

    // Speaker
    if (debateSpeakerText_.tex.SRV()) {
        Render::RectF dst{
            debateDraw_.speaker.x, debateDraw_.speaker.y,
            static_cast<float>(debateSpeakerText_.w),
            static_cast<float>(debateSpeakerText_.h)
        };
        drawList.PushSprite(Render::Layer::HUD, debateSpeakerText_.tex.SRV(), dst);
    }

    // Body
    if (debateBodyText_.tex.SRV()) {
        Render::RectF dst{
            debateDraw_.body.x, debateDraw_.body.y,
            static_cast<float>(debateBodyText_.w),
            static_cast<float>(debateBodyText_.h)
        };
        drawList.PushSprite(Render::Layer::HUD, debateBodyText_.tex.SRV(), dst);
    }

    // Spans
    for (size_t i = 0; i < debateSpanTexts_.size(); ++i) {
        const auto& spanText = debateSpanTexts_[i];
        if (!spanText.tex.SRV()) continue;

        const auto& spanReq = debateDraw_.spans[i];
        Render::RectF dst{
            spanReq.x, spanReq.y,
            static_cast<float>(spanText.w),
            static_cast<float>(spanText.h)
        };
        drawList.PushSprite(Render::Layer::HUD, spanText.tex.SRV(), dst);
    }

    // Options
    for (size_t i = 0; i < debateOptionTexts_.size(); ++i) {
        const auto& optText = debateOptionTexts_[i];
        if (!optText.tex.SRV()) continue;

        const auto& optReq = debateDraw_.options[i];
        Render::RectF dst{
            optReq.x, optReq.y,
            static_cast<float>(optText.w),
            static_cast<float>(optText.h)
        };
        drawList.PushSprite(Render::Layer::HUD, optText.tex.SRV(), dst);
    }

    // Highlights
    for (const auto& hlReq : debateDraw_.highlightRects) {
        drawList.PushSprite(Render::Layer::HUD, white1x1_.SRV(),
            hlReq, 0.0f, {}, debateDraw_.highlightTint);
    }
}

void GameScene::BuildDrawList(Render::DrawList& drawList, uint32_t canvasW, uint32_t canvasH) {
    (void)canvasW; (void)canvasH;

    if (!player_) return;

    switch (player_->View().nodeType) {
    case Game::Story::NodeType::VN:
    case Game::Story::NodeType::Error:
    case Game::Story::NodeType::BE:
        DrawVN(drawList);
        break;
    case Game::Story::NodeType::Debate:
        DrawDebate(drawList);
        break;
    case Game::Story::NodeType::Present:
        // placeholder for future Present mode
        break;
    default:
        break;
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

// ======================== End of Render Functions ==========================
    
} // namespace Salt2D::App
