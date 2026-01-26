// RHI/DX11/DX11Texture2D.cpp
#include "DX11Texture2D.h"

#include "DX11Common.h"
#include "DX11Device.h"

#include <stdexcept>

namespace Salt2D::RHI::DX11 {

DX11Texture2D DX11Texture2D::CreateRGBA8(
    const DX11Device& device,
    uint32_t width,
    uint32_t height,
    const void* rgbaData,
    uint32_t rowPitchBytes
) {
    if (!rgbaData) throw std::invalid_argument("RGBA data pointer is null.");
    if (rowPitchBytes < width * 4) throw std::invalid_argument("Row pitch is too small for the given width.");

    auto d3dDevice = device.GetDevice();

    DX11Texture2D texture;
    texture.width_  = width;
    texture.height_ = height;
    texture.format_ = DXGI_FORMAT_R8G8B8A8_UNORM;

    D3D11_TEXTURE2D_DESC texDesc = {};
    texDesc.Width              = width;
    texDesc.Height             = height;
    texDesc.MipLevels          = 1;
    texDesc.ArraySize          = 1;
    texDesc.Format             = texture.format_;
    texDesc.SampleDesc.Count   = 1;
    texDesc.Usage              = D3D11_USAGE_IMMUTABLE;
    texDesc.BindFlags          = D3D11_BIND_SHADER_RESOURCE;

    D3D11_SUBRESOURCE_DATA initData = {};
    initData.pSysMem          = rgbaData;
    initData.SysMemPitch      = rowPitchBytes;

    ThrowIfFailed(d3dDevice->CreateTexture2D(
        &texDesc, &initData, &texture.texture_),
        "Failed to create RGBA8 texture.");

    ThrowIfFailed(d3dDevice->CreateShaderResourceView(
        texture.texture_.Get(), nullptr, &texture.srv_),
        "Failed to create shader resource view for RGBA8 texture.");

    return texture;
}

DX11Texture2D DX11Texture2D::CreateRenderTarget(
    const DX11Device& device,
    uint32_t width,
    uint32_t height,
    DXGI_FORMAT format
) {
    auto d3dDevice = device.GetDevice();

    DX11Texture2D texture;
    texture.width_  = width;
    texture.height_ = height;
    texture.format_ = format;

    D3D11_TEXTURE2D_DESC texDesc = {};
    texDesc.Width              = width;
    texDesc.Height             = height;
    texDesc.MipLevels          = 1;
    texDesc.ArraySize          = 1;
    texDesc.Format             = texture.format_;
    texDesc.SampleDesc.Count   = 1;
    texDesc.Usage              = D3D11_USAGE_DEFAULT;
    texDesc.BindFlags          = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;

    ThrowIfFailed(d3dDevice->CreateTexture2D(
        &texDesc, nullptr, &texture.texture_),
        "Failed to create render target texture.");

    ThrowIfFailed(d3dDevice->CreateRenderTargetView(
        texture.texture_.Get(), nullptr, &texture.rtv_),
        "Failed to create render target view.");

    ThrowIfFailed(d3dDevice->CreateShaderResourceView(
        texture.texture_.Get(), nullptr, &texture.srv_),
        "Failed to create shader resource view for render target texture.");

    return texture;
}

} // namespace Salt2D::RHI::DX11
