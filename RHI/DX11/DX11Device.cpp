// RHI/DX11/DX11Device.cpp
#include "DX11Device.h"
#include "DX11Common.h"

using Microsoft::WRL::ComPtr;

namespace Salt2D::RHI::DX11 {

DX11Device::DX11Device(const ComPtr<IDXGIAdapter1>& adapter) {
    adapter_ = adapter;

    UINT flags = 0;

    flags |= D3D11_CREATE_DEVICE_BGRA_SUPPORT;

#if defined(_DEBUG)
    flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

    const D3D_FEATURE_LEVEL featureLevels[] = {
        D3D_FEATURE_LEVEL_11_1,
        D3D_FEATURE_LEVEL_11_0,
    };

    HRESULT hr;

    if (adapter_) {
        hr = D3D11CreateDevice(
            adapter_.Get(), D3D_DRIVER_TYPE_UNKNOWN,
            nullptr, flags, featureLevels, _countof(featureLevels),
            D3D11_SDK_VERSION, &device_, &featureLevel_, &context_
        );
    } else {
        hr = D3D11CreateDevice(
            nullptr, D3D_DRIVER_TYPE_HARDWARE,
            nullptr, flags, featureLevels, _countof(featureLevels),
            D3D11_SDK_VERSION, &device_, &featureLevel_, &context_
        );
    }

#if defined(_DEBUG)
    if (FAILED(hr)) {
        flags &= ~D3D11_CREATE_DEVICE_DEBUG;

        if (adapter_) {
            hr = D3D11CreateDevice(
                adapter_.Get(), D3D_DRIVER_TYPE_UNKNOWN,
                nullptr, flags, featureLevels, _countof(featureLevels),
                D3D11_SDK_VERSION, &device_, &featureLevel_, &context_
            );
        } else {
            hr = D3D11CreateDevice(
                nullptr, D3D_DRIVER_TYPE_HARDWARE,
                nullptr, flags, featureLevels, _countof(featureLevels),
                D3D11_SDK_VERSION, &device_, &featureLevel_, &context_
            );
        }
    } else {
        debugLayerEnabled_ = true;
    }
#endif

    ThrowIfFailed(hr, "Failed to create D3D11 device.");

    ComPtr<IDXGIDevice> dxgiDevice;
    ThrowIfFailed(device_.As(&dxgiDevice),"Failed to get IDXGIDevice from ID3D11Device.");

    ComPtr<IDXGIAdapter> dxgiAdapter;
    ThrowIfFailed(dxgiDevice->GetAdapter(&dxgiAdapter), "Failed to get IDXGIAdapter from IDXGIDevice.");

    if (!adapter_) {
        ThrowIfFailed(dxgiAdapter.As(&adapter_), "Failed to query IDXGIAdapter1.");
    }

    ThrowIfFailed(dxgiAdapter->GetParent(
        IID_PPV_ARGS(&factory_)),
        "Failed to get IDXGIFactory2 from IDXGIAdapter.");
}

} // namespace Salt2D::RHI::DX11
