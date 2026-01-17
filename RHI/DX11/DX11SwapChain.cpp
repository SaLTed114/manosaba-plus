// RHI/DX11/DX11SwapChain.cpp
#include "DX11SwapChain.h"
#include "DX11Device.h"
#include "DX11Common.h"

#include <iostream>

using Microsoft::WRL::ComPtr;

namespace Salt2D::RHI::DX11 {

DX11SwapChain::DX11SwapChain(
    const DX11Device& device,
    HWND hwnd, uint32_t width, uint32_t height,
    uint32_t bufferCount,
    DXGI_FORMAT format
) : device(device), width_(width), height_(height),
    bufferCount_(bufferCount), format_(format),
    hwnd_(hwnd)
{
    CreateSwapChain();
    CreateBackBufferRTV();
}

void DX11SwapChain::Present(bool vsync) {
    if (!swapChain_) return;
    ThrowIfFailed(swapChain_->Present(vsync ? 1 : 0, 0), "Failed to present swap chain.");
}

UINT DX11SwapChain::GetCurrentBackBufferIndex() const {
    ComPtr<IDXGISwapChain3> swapChain3;
    ThrowIfFailed(swapChain_.As(&swapChain3), "Failed to query IDXGISwapChain3 interface.");
    return swapChain3->GetCurrentBackBufferIndex();
}

void DX11SwapChain::Resize(uint32_t width, uint32_t height) {
    width_  = width;
    height_ = height;

    if (!swapChain_) return;
    if (width_ == 0 || height_ == 0) {
        ReleaseBackBufferRTV();
        return;
    }

    ReleaseBackBufferRTV();

    ThrowIfFailed(swapChain_->ResizeBuffers(
        bufferCount_, width_, height_, format_, 0),
        "Failed to resize swap chain buffers.");

    CreateBackBufferRTV();
}

void DX11SwapChain::CreateSwapChain() {
    ComPtr<IDXGIFactory2> factory = device.GetFactory();
    if (!factory) {
        throw std::runtime_error("DX11SwapChain: DXGIFactory2 is null.");
    }

    DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
    swapChainDesc.Width              = width_;
    swapChainDesc.Height             = height_;
    swapChainDesc.Format             = format_;
    swapChainDesc.Stereo             = FALSE;
    swapChainDesc.SampleDesc.Count   = 1;
    swapChainDesc.SampleDesc.Quality = 0;
    swapChainDesc.BufferUsage        = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDesc.BufferCount        = bufferCount_;
    swapChainDesc.Scaling            = DXGI_SCALING_STRETCH;
    swapChainDesc.SwapEffect         = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    swapChainDesc.AlphaMode          = DXGI_ALPHA_MODE_UNSPECIFIED;
    swapChainDesc.Flags              = 0;

    ComPtr<IDXGISwapChain1> swapChain;
    ThrowIfFailed(factory->CreateSwapChainForHwnd(
        device.GetDevice(), hwnd_,
        &swapChainDesc, nullptr, nullptr, &swapChain),
        "Failed to create DX11 swap chain.");

    ThrowIfFailed(swapChain.As(&swapChain_),"Failed to query IDXGISwapChain1 interface.");

    factory->MakeWindowAssociation(hwnd_, DXGI_MWA_NO_ALT_ENTER);
}

void DX11SwapChain::CreateBackBufferRTV() {
    ThrowIfFailed(swapChain_->GetBuffer(
        0, IID_PPV_ARGS(&backBuffer_)),
        "Failed to get back buffer from swap chain.");

    D3D11_RENDER_TARGET_VIEW_DESC rtvDesc = {};
    rtvDesc.Format             = format_;
    rtvDesc.ViewDimension      = D3D11_RTV_DIMENSION_TEXTURE2D;
    rtvDesc.Texture2D.MipSlice = 0;

    ThrowIfFailed(device.GetDevice()->CreateRenderTargetView(
        backBuffer_.Get(), &rtvDesc, &backBufferRTV_),
        "Failed to create render target view for back buffer.");
}

void DX11SwapChain::ReleaseBackBufferRTV() {
    backBufferRTV_.Reset();
    backBuffer_.Reset();
}

} // namespace Salt2D::RHI::DX11
