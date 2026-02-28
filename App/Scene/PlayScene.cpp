// App/Scene/PlayScene.cpp
#include "PlayScene.h"
#include "Render/Passes/SceneSpritePass.h"
#include "Render/Passes/ComposePass.h"

#include <Windows.h>
#include <vector>
#include <algorithm>

namespace Salt2D::App {

PlayScene::PlayScene(Utils::DiskFileSystem& fs) : fs_(fs), flow_(fs) {}

void PlayScene::Initialize(Render::DX11Renderer& renderer) {
    auto device = renderer.Device();

    text_.Initialize();
    text_.ClearCache();

    texService_.Clear();
    texCatalog_.Clear();

    // white 1x1 texture
    {
        uint8_t whitePixel[4] = { 255, 255, 255, 255 };
        white1x1_ = RHI::DX11::DX11Texture2D::CreateRGBA8(device, 1, 1, whitePixel, 4);
    }

    // chekerboard texture placeholder
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
    
    texService_.Register(Game::UI::TextureId::White,   white1x1_.SRV(), 1,  1);
    texService_.Register(Game::UI::TextureId::Checker, checker_.SRV(),  64, 64);
    texService_.SetMissing(Game::UI::TextureId::Checker);

    texCatalog_.SetAssetsRoot("Assets/");
    texCatalog_.SetMissing({ checker_.SRV(), 64, 64, true });

    screens_.Initialize();

    BuildDefaultChapters();
    flow_.Start();

    ProcessFlowEvents(device);

    presReg_.Initialize(texCatalog_);
}

void PlayScene::BuildDefaultChapters() {
    using namespace Game::Flow;
    std::vector<ChapterDef> chapters = {
        {"demo_novel", ChapterKind::Novel, "Assets/Story/DemoNovel/", "demo_novel.graph.json", "n0_intro", ""},
        {"demo_trial", ChapterKind::Trial, "Assets/Story/DemoTrial/", "demo_trial.graph.json", "n0_intro", "inquisition"},
    };
    flow_.SetChapters(std::move(chapters));
}

void PlayScene::BindToFlowSession() {
    UnbindFromFlowSession();

    if (!flow_.HasSession()) return;
    auto* session = flow_.Session();
    if (!session) return;

    screens_.Bind({ &session->Player(), &session->History(), &session->Tables() });
    screensBound_ = true;
}

void PlayScene::UnbindFromFlowSession() {
    if (!screensBound_) return;

    screens_.Unbind();
    screensBound_ = false;
}

void PlayScene::ProcessFlowEvents(const RHI::DX11::DX11Device& device) {
    while (auto evtOpt = flow_.ConsumeEvent()) {
        auto evt = *evtOpt;

        using Game::Flow::FlowEvent;
        switch (evt) {
        case FlowEvent::SessionCleared:
        case FlowEvent::Finished:
            UnbindFromFlowSession();
            presReg_.OnSessionCleared();
            break;
        case FlowEvent::SessionCreated:
            BindToFlowSession();
            presReg_.OnSessionCreated(device, *flow_.CurrentChapter(), *flow_.Session());
            break;
        case FlowEvent::None:
        default:
            break;
        }
    }
}

void PlayScene::Update(
    const RHI::DX11::DX11Device& device,
    const Core::FrameTime& ft,
    const Core::InputState& in,
    uint32_t canvasW, uint32_t canvasH
) {
    if (flow_.State() == Game::Flow::FlowState::GateReady) {
        // tmp keyboard hotkey to confirm gate
        if (in.Pressed(Core::Key::Enter) || in.Pressed(Core::Key::Space)) {
            flow_.ConfirmGate();
        }
    }

    if (flow_.State() == Game::Flow::FlowState::Finished) {
        // tmp keyboard hotkey to restart
        if (in.Pressed(Core::Key::Enter) || in.Pressed(Core::Key::Space)) {
            flow_.Start();
        }
    }

    if (flow_.HasSession()) {
        auto* player = flow_.Player();
        if (player)  player->Tick(ft.dtSec);

        presReg_.Tick(*player, ft);
        screens_.Tick(ft, in, canvasW, canvasH);
        screens_.Bake(device, text_);
        screens_.PostBake(in, canvasW, canvasH);
    }

    flow_.Tick(ft);
    ProcessFlowEvents(device);
}

void PlayScene::FillFrameBlackboard(Render::FrameBlackboard& frame, uint32_t sceneW, uint32_t sceneH) {
    presReg_.FillFrameBlackboard(frame, sceneW, sceneH);
}

void PlayScene::BuildDrawList(Render::DrawList& drawList, uint32_t canvasW, uint32_t canvasH) {
    (void)canvasW; (void)canvasH;

    presReg_.EmitSceneDraw(drawList, canvasW, canvasH);
    screens_.EmitDraw(drawList, texService_);
}

void PlayScene::BuildPlan(Render::RenderPlan& plan, const Render::DrawList& drawList) {
    using namespace Render;

    auto bg  = drawList.Sprites(Layer::Background); // probably empty
    auto hud = drawList.Sprites(Layer::HUD);

    auto p0 = std::make_unique<SpritePass>("Scene_BG_2D", Target::Scene, DepthMode::Off, BlendMode::Alpha, bg);
    p0->SetClearScene(0.15f, 0.15f, 0.18f, 1.0f);
    plan.passes.push_back(std::move(p0));

    presReg_.BuildScenePasses(plan, drawList);

    auto p1 = std::make_unique<ComposePass>("Compose");
    plan.passes.push_back(std::move(p1));

    auto p2 = std::make_unique<SpritePass>("HUD", Target::BackBuffer, DepthMode::Off, BlendMode::Alpha, hud);
    plan.passes.push_back(std::move(p2));
}

} // namespace Salt2D::App
