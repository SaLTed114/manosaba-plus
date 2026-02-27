// App/Application.cpp
#include "Application.h"
#include "Scene/DemoScene.h"
#include "Scene/GameScene.h"
#include "Scene/PlayScene.h"

#include <Windows.h>
#include <objbase.h>
#include <iostream>
#include <stdexcept>

namespace Salt2D::App {

Application::Application(const char* title, uint32_t width, uint32_t height) {
    SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
    CoInitializeEx(nullptr, COINIT_MULTITHREADED);

    window_ = std::make_unique<Core::Win32Window>(title, width, height);

    window_->GetClientSize(canvasW_, canvasH_);
    window_->setOnResized([this](uint32_t w, uint32_t h) { OnResized(w, h); });

    renderer_ = std::make_unique<Render::DX11Renderer>(window_->GetHWND(), canvasW_, canvasH_);

    // scene_ = std::make_unique<DemoScene>();
    // scene_->Initialize(*renderer_);

    // scene_ = std::make_unique<GameScene>(fs_);
    // scene_->Initialize(*renderer_);

    scene_ = std::make_unique<PlayScene>(fs_);
    scene_->Initialize(*renderer_);
}

Application::~Application() {
    scene_.reset();
    renderer_.reset();
    window_.reset();
    
    CoUninitialize();
}

void Application::OnResized(uint32_t w, uint32_t h) {
    if (w == 0 || h == 0 || w > 32768 || h > 32768) return;
    
    canvasW_ = w;
    canvasH_ = h;
    renderer_->Resize(w, h);
}

void Application::Tick(const Core::FrameTime& ft, const Core::InputState& in) {
    if (canvasW_ == 0 || canvasH_ == 0) { return; }
    
    scene_->Update(renderer_->Device(), ft, in, canvasW_, canvasH_);

    drawList_.Clear();
    scene_->BuildDrawList(drawList_, canvasW_, canvasH_);
    drawList_.Sort();

    uint32_t sceneW = renderer_->GetSceneW();
    uint32_t sceneH = renderer_->GetSceneH();
    scene_->FillFrameBlackboard(frame_, sceneW, sceneH);

    plan_.Clear();
    scene_->BuildPlan(plan_, drawList_);
}

void Application::Run() {
    clock_.Reset();

    // tmp: clear after runtime settings are implemented
    static const struct { uint32_t w, h; } testSizes[] = {
        { 1280, 720 },
        { 1920, 1080 },
        { 2560, 1440 },
        { 3840, 2160 },
    };
    static int resizeIndex = 2;

    while (window_->ProcessMessages()) {
        auto& in = window_->GetInputState();

        // tmp: clear after runtime settings are implemented
        if (in.Pressed(VK_F6)) {
            resizeIndex = (resizeIndex + 1) % std::size(testSizes);
            const auto& sz = testSizes[resizeIndex];
            window_->SetClientSize(sz.w, sz.h);
        }
        if (in.Pressed(VK_F11)) {
            window_->ToggleBorderlessFullscreen();
        }

        auto ft = clock_.Tick();

        Tick(ft, in);

        renderer_->ExecutePlan(plan_, frame_);
        renderer_->Present(vsync_);
    }
}


} // namespace Salt2D::App