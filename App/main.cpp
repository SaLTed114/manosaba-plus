// App/main.cpp
#include "Utils/ConsoleUtils.h"
#include "Core/Window/Win32Window.h"
#include "Render/DX11Renderer.h"
#include "Application.h"

#include <Windows.h>
#include <iostream>

using namespace Salt2D;

int WINAPI wWinMain(HINSTANCE, HINSTANCE, PWSTR, int) {
    Utils::AttachParentConsoleAndRedirectIO();

    try {
        App::Application app("manosaba+", 1920, 1080);
        app.Run();

    } catch (const std::exception& e) {
        std::cerr << "Fatal Error: " << e.what() << std::endl;
        MessageBoxA(nullptr, e.what(), "Fatal Error", MB_OK | MB_ICONERROR);
        return -1;
    }

    return 0;
}
