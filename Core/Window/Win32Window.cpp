// Core/Window/Win32Window.cpp
#include "Win32Window.h"

#include <stdexcept>
#include <iostream>

namespace Salt2D::Core {

void Win32Window::GetClientSize(uint32_t& width, uint32_t& height) const {
    RECT rect{};
    GetClientRect(hwnd_, &rect);
    width  = static_cast<uint32_t>(rect.right - rect.left);
    height = static_cast<uint32_t>(rect.bottom - rect.top);
}

Win32Window::Win32Window(const std::string& title, int width, int height)
    : width_(width), height_(height) {

    WNDCLASS wc = {};
    wc.lpfnWndProc   = Win32Window::WndProc;
    wc.hInstance     = GetModuleHandle(nullptr);
    wc.lpszClassName = L"SaltRTWindowClass";
    wc.hCursor       = LoadCursor(nullptr, IDC_ARROW);

    RegisterClass(&wc);

    RECT rect = {0, 0, width_, height_};
    AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, FALSE);

    std::wstring wtitle(title.begin(), title.end());
    hwnd_ = CreateWindowEx(
        0, wc.lpszClassName, wtitle.c_str(),
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        rect.right - rect.left, rect.bottom - rect.top,
        nullptr, nullptr, wc.hInstance, this
    );

    if (!hwnd_) {
        throw std::runtime_error("Failed to create Win32 window.");
    }

    ShowWindow(hwnd_, SW_SHOW);
    UpdateWindow(hwnd_);
}

Win32Window::~Win32Window() {
    if (hwnd_) {
        DestroyWindow(hwnd_);
        hwnd_ = nullptr;
    }
}

bool Win32Window::ProcessMessages() {
    MSG msg;
    while (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE)) {
        if (msg.message == WM_QUIT) return false;
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }
    return true;
}

LRESULT CALLBACK Win32Window::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (msg == WM_NCCREATE) {
        auto* cs = reinterpret_cast<CREATESTRUCTW*>(lParam);
        auto* self = reinterpret_cast<Win32Window*>(cs->lpCreateParams);
        SetWindowLongPtrW(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(self));
        self->hwnd_ = hWnd;
    }

    auto* self = reinterpret_cast<Win32Window*>(GetWindowLongPtrW(hWnd, GWLP_USERDATA));
    if (self) return self->HandleMessage(msg, wParam, lParam);

    return DefWindowProcW(hWnd, msg, wParam, lParam);
}

LRESULT Win32Window::HandleMessage(UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;

    case WM_SIZE: {
        width_  = LOWORD(lParam);
        height_ = HIWORD(lParam);
        if (onResized_) onResized_(static_cast<uint32_t>(width_), static_cast<uint32_t>(height_));
        return 0;
    }

    case WM_DPICHANGED: {
        RECT* suggested = reinterpret_cast<RECT*>(lParam);
        SetWindowPos(hwnd_, nullptr,
            suggested->left, suggested->top,
            suggested->right - suggested->left,
            suggested->bottom - suggested->top,
            SWP_NOZORDER | SWP_NOACTIVATE);

        float dpiScale = static_cast<float>(HIWORD(wParam)) / 96.0f;
        if (onDpiChanged_) onDpiChanged_(dpiScale);
        return 0;
    }

    default:
        break;
    }

    return DefWindowProcW(hwnd_, msg, wParam, lParam);
}


} // namespace Salt2D::Core
