// App/main.cpp
#include "Utils/ConsoleUtils.h"
#include "Core/Window/Win32Window.h"
#include "Render/DX11Renderer.h"

#include <Windows.h>
#include <iostream>

using namespace Salt2D;

int WINAPI wWinMain(HINSTANCE, HINSTANCE, PWSTR, int) {
    Utils::AttachParentConsoleAndRedirectIO();

    try {
        SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
        Core::Win32Window window("manosaba+", 1920, 1080);

        uint32_t cw = 0, ch = 0;
        window.GetClientSize(cw, ch);

        Render::DX11Renderer renderer(window.GetHWND(), cw, ch);

        while (window.ProcessMessages()) {
            renderer.RenderFrame(true);
        }

    } catch (const std::exception& e) {
        std::cerr << "Fatal Error: " << e.what() << std::endl;
        MessageBoxA(nullptr, e.what(), "Fatal Error", MB_OK | MB_ICONERROR);
        return -1;
    }

    return 0;
}
