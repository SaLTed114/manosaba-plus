// RHI/DX11/DX11Texture2D.cpp
#include "DX11Texture2D.h"

#include "DX11Common.h"
#include "DX11Device.h"

#include <stdexcept>

namespace Salt2D::RHI::DX11 {

namespace {

static uint32_t BytesPerPixel(DXGI_FORMAT format) {
    switch (format) {
        case DXGI_FORMAT_R8G8B8A8_UNORM:
        case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
        case DXGI_FORMAT_B8G8R8A8_UNORM:
        case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB:
            return 4;
        default:
            throw std::invalid_argument("Unsupported DXGI_FORMAT for BytesPerPixel.");
    }
}

} // Anonymous namespace

DX11Texture2D DX11Texture2D::CreateImmutableSRV(
    const DX11Device& device,
    uint32_t width,
    uint32_t height,
    DXGI_FORMAT format,
    const void* initData,
    uint32_t rowPitchBytes
) {
    if (!initData) throw std::invalid_argument("Initial data pointer is null.");
    if (rowPitchBytes < width * BytesPerPixel(format)) throw std::invalid_argument("Row pitch is too small for the given width and format.");

    auto d3dDevice = device.GetDevice();

    DX11Texture2D texture;
    texture.width_  = width;
    texture.height_ = height;
    texture.format_ = format;
    texture.usage_  = D3D11_USAGE_IMMUTABLE;
    texture.cpuAccessFlags_ = 0;

    D3D11_TEXTURE2D_DESC texDesc = {};
    texDesc.Width              = width;
    texDesc.Height             = height;
    texDesc.MipLevels          = 1;
    texDesc.ArraySize          = 1;
    texDesc.Format             = texture.format_;
    texDesc.SampleDesc.Count   = 1;
    texDesc.Usage              = texture.usage_;
    texDesc.BindFlags          = D3D11_BIND_SHADER_RESOURCE;
    texDesc.CPUAccessFlags     = texture.cpuAccessFlags_;

    D3D11_SUBRESOURCE_DATA d3dInitData = {};
    d3dInitData.pSysMem          = initData;
    d3dInitData.SysMemPitch      = rowPitchBytes;

    ThrowIfFailed(d3dDevice->CreateTexture2D(
        &texDesc, &d3dInitData, &texture.texture_),
        "Failed to create immutable shader resource view texture.");

    ThrowIfFailed(d3dDevice->CreateShaderResourceView(
        texture.texture_.Get(), nullptr, &texture.srv_),
        "Failed to create shader resource view for immutable texture.");

    return texture;
}

DX11Texture2D DX11Texture2D::CreateDynamicSRV(
    const DX11Device& device,
    uint32_t width,
    uint32_t height,
    DXGI_FORMAT format
) {
    BytesPerPixel(format); // Validate format

    auto d3dDevice = device.GetDevice();

    DX11Texture2D texture;
    texture.width_  = width;
    texture.height_ = height;
    texture.format_ = format;
    texture.usage_  = D3D11_USAGE_DYNAMIC;
    texture.cpuAccessFlags_ = D3D11_CPU_ACCESS_WRITE;

    D3D11_TEXTURE2D_DESC texDesc = {};
    texDesc.Width              = width;
    texDesc.Height             = height;
    texDesc.MipLevels          = 1;
    texDesc.ArraySize          = 1;
    texDesc.Format             = texture.format_;
    texDesc.SampleDesc.Count   = 1;
    texDesc.Usage              = texture.usage_;
    texDesc.BindFlags          = D3D11_BIND_SHADER_RESOURCE;
    texDesc.CPUAccessFlags     = texture.cpuAccessFlags_;

    ThrowIfFailed(d3dDevice->CreateTexture2D(
        &texDesc, nullptr, &texture.texture_),
        "Failed to create dynamic shader resource view texture.");

    ThrowIfFailed(d3dDevice->CreateShaderResourceView(
        texture.texture_.Get(), nullptr, &texture.srv_),
        "Failed to create shader resource view for dynamic texture.");

    return texture;
}

void DX11Texture2D::UpdateDynamic(
    ID3D11DeviceContext* context,
    const void* rgbaData,
    uint32_t rowPitchBytes
) {
    if (!context) throw std::invalid_argument("Device context pointer is null.");
    if (!rgbaData) throw std::invalid_argument("RGBA data pointer is null.");
    if (!texture_) throw std::runtime_error("Texture is not initialized.");
    if (usage_ != D3D11_USAGE_DYNAMIC) throw std::runtime_error("Texture is not dynamic.");

    const uint32_t bpp = BytesPerPixel(format_);
    const uint32_t requiredRowPitch = width_ * bpp;
    if (rowPitchBytes < requiredRowPitch) throw std::invalid_argument("Row pitch is too small for the given width and format.");

    D3D11_MAPPED_SUBRESOURCE mappedResource;
    ThrowIfFailed(context->Map(texture_.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource),
        "Failed to map dynamic texture for update.");

    const uint8_t* srcData = reinterpret_cast<const uint8_t*>(rgbaData);
    uint8_t* dstData = reinterpret_cast<uint8_t*>(mappedResource.pData);

    for (uint32_t row = 0; row < height_; ++row) {
        std::memcpy(
            dstData + row * mappedResource.RowPitch,
            srcData + row * rowPitchBytes,
            requiredRowPitch);
    }

    context->Unmap(texture_.Get(), 0);
}

DX11Texture2D DX11Texture2D::CreateRGBA8(
    const DX11Device& device,
    uint32_t width,
    uint32_t height,
    const void* rgbaData,
    uint32_t rowPitchBytes
) {
    return CreateImmutableSRV(
        device,
        width,
        height,
        DXGI_FORMAT_R8G8B8A8_UNORM,
        rgbaData,
        rowPitchBytes);
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
