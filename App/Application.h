// App/Application.h
#ifndef APP_APPLICATION_H
#define APP_APPLICATION_H

#include <cstdint>
#include <memory>

#include "Core/Window/Win32Window.h"
#include "Core/Time/FrameClock.h"

#include "Render/DX11Renderer.h"
#include "Render/RenderPlan.h"
#include "Render/Draw/DrawList.h"

namespace Salt2D::App {

class DemoScene;

class Application {
public:
    Application(const char* title, uint32_t width, uint32_t height);
    ~Application();

    Application(const Application&) = delete;
    Application& operator=(const Application&) = delete;

    void Run();

private:
    void OnResized(uint32_t w, uint32_t h);
    void Tick(const Core::FrameTime& ft);

private:
    std::unique_ptr<Core::Win32Window> window_;
    std::unique_ptr<Render::DX11Renderer> renderer_;
    std::unique_ptr<DemoScene> scene_;

    Render::DrawList drawList_;
    Render::RenderPlan plan_;

    uint32_t canvasW_ = 0;
    uint32_t canvasH_ = 0;

    Core::FrameClock clock_;
    bool vsync_ = true;
};

} // namespace Salt2D::App

#endif // APP_APPLICATION_H
