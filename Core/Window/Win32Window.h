// Core/Window/Win32Window.h
#ifndef CORE_WINDOW_WIN32WINDOW_H
#define CORE_WINDOW_WIN32WINDOW_H

#include <Windows.h>
#include <string>
#include <functional>
#include <cstdint>

namespace Salt2D::Core {

class Win32Window {
public:
    using ResizedCallback    = std::function<void(uint32_t width, uint32_t height)>;
    using DpiChangedCallback = std::function<void(float dpi)>;

public:
    Win32Window(const std::string& title, int width, int height);
    ~Win32Window();

    Win32Window(const Win32Window&) = delete;
    Win32Window& operator=(const Win32Window&) = delete;

    bool ProcessMessages();

public:
    HWND GetHWND()  const { return hwnd_; }

    int GetWidth()  const { return width_; }
    int GetHeight() const { return height_; }
    void GetClientSize(uint32_t& width, uint32_t& height) const;

    void setOnResized(ResizedCallback callback) { onResized_ = std::move(callback); }
    void setOnDpiChanged(DpiChangedCallback callback) { onDpiChanged_ = std::move(callback); }


private:
    static LRESULT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
    LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);

private:
    HWND hwnd_ = nullptr;
    int width_  = 0;
    int height_ = 0;

    ResizedCallback onResized_ = nullptr;
    DpiChangedCallback onDpiChanged_ = nullptr;
};

} // namespace Salt2D::Core

#endif // CORE_WINDOW_WIN32WINDOW_H
