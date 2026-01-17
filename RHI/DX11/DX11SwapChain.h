// RHI/DX11/DX11SwapChain.h
#ifndef RHI_DX11_SWAPCHAIN_H
#define RHI_DX11_SWAPCHAIN_H

#include <cstdint>
#include <wrl/client.h>
#include <dxgi1_6.h>
#include <d3d11.h>

namespace Salt2D::RHI::DX11 {

class DX11Device;

class DX11SwapChain {
public:
    DX11SwapChain(
        const DX11Device& device,
        HWND hwnd, uint32_t width, uint32_t height,
        uint32_t bufferCount = 2,
        DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM
    );

    void Present(bool vsync = true);

    void Resize(uint32_t width, uint32_t height);

    UINT GetCurrentBackBufferIndex() const;

    Microsoft::WRL::ComPtr<IDXGISwapChain1> GetSwapChain()  const { return swapChain_; }
    Microsoft::WRL::ComPtr<ID3D11Texture2D> GetBackBuffer() const { return backBuffer_; }
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView> GetBackBufferRTV() const { return backBufferRTV_; }

    const uint32_t GetWidth()  const { return width_; }
    const uint32_t GetHeight() const { return height_; }

    const uint32_t GetBufferCount() const { return bufferCount_; }
    const DXGI_FORMAT GetFormat()   const { return format_; }

    HWND GetHwnd() const { return hwnd_; }

private:
    void CreateSwapChain();
    void CreateBackBufferRTV();
    void ReleaseBackBufferRTV();

private:
    const DX11Device& device;

    uint32_t width_  = 0;
    uint32_t height_ = 0;
    uint32_t bufferCount_ = 2;
    DXGI_FORMAT format_ = DXGI_FORMAT_R8G8B8A8_UNORM;
    HWND hwnd_ = nullptr;

    Microsoft::WRL::ComPtr<IDXGISwapChain1> swapChain_;
    Microsoft::WRL::ComPtr<ID3D11Texture2D> backBuffer_;
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView> backBufferRTV_;
};

} // namespace Salt2D::RHI::DX11

#endif // RHI_DX11_SWAPCHAIN_H
