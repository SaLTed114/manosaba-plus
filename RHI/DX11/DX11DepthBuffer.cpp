// RHI/DX11/DX11DepthBuffer.cpp
#include "DX11DepthBuffer.h"
#include "DX11Device.h"
#include "DX11Common.h"

namespace Salt2D::RHI::DX11 {

DX11DepthBuffer DX11DepthBuffer::Create(
    const DX11Device& device,
    uint32_t width,
    uint32_t height,
    DXGI_FORMAT format)
{
    DX11DepthBuffer depthBuffer;
    depthBuffer.CreateResources(device, width, height, format);
    return depthBuffer;
}

void DX11DepthBuffer::Resize(
    const DX11Device& device,
    uint32_t width,
    uint32_t height)
{
    if (width == width_ && height == height_ && dsv_) return;
    CreateResources(device, width, height,
        format_ == DXGI_FORMAT_UNKNOWN ? DXGI_FORMAT_D24_UNORM_S8_UINT : format_);
}

void DX11DepthBuffer::Clear(
    ID3D11DeviceContext* context,
    float depth,
    uint8_t stencil) const
{
    if (!dsv_) return;
    context->ClearDepthStencilView(dsv_.Get(),
        D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
        depth, stencil);
}

void DX11DepthBuffer::CreateResources(
    const DX11Device& device,
    uint32_t width,
    uint32_t height,
    DXGI_FORMAT format
) {
    auto d3dDevice = device.GetDevice();

    width_  = width;
    height_ = height;
    texture_.Reset();
    dsv_.Reset();

    D3D11_TEXTURE2D_DESC texDesc{};
    texDesc.Width              = width;
    texDesc.Height             = height;
    texDesc.MipLevels          = 1;
    texDesc.ArraySize          = 1;
    texDesc.Format             = format;
    texDesc.SampleDesc.Count   = 1;
    texDesc.Usage              = D3D11_USAGE_DEFAULT;
    texDesc.BindFlags          = D3D11_BIND_DEPTH_STENCIL;

    ThrowIfFailed(d3dDevice->CreateTexture2D(&texDesc, nullptr, texture_.GetAddressOf()),
        "DX11DepthBuffer::CreateResources: CreateTexture2D for depth buffer failed.");

    Microsoft::WRL::ComPtr<ID3D11DepthStencilView> dsv;
    ThrowIfFailed(d3dDevice->CreateDepthStencilView(
        texture_.Get(), nullptr, dsv.GetAddressOf()),
        "DX11DepthBuffer::CreateResources: CreateDepthStencilView failed.");
}

} // namespace Salt2D::RHI::DX11
