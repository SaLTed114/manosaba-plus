// Render/Resources/Texture2D.cpp
#include "Texture2D.h"
#include "RHI/DX11/DX11Common.h"

#include <stdexcept>

namespace Salt2D::Render {

Texture2D Texture2D::CreateRGBA8(
    ID3D11Device* device,
    uint32_t width,
    uint32_t height,
    const void* rgbaData,
    uint32_t rowPitchBytes
) {
    if (!device) throw std::runtime_error("Texture2D::CreateRGBA8: device is null.");
    if (!rgbaData) throw std::runtime_error("Texture2D::CreateRGBA8: data is null.");
    if (rowPitchBytes < width * 4) throw std::runtime_error("Texture2D::CreateRGBA8: rowPitchBytes is too small.");

    Texture2D texture;
    texture.width = width;
    texture.height = height;

    D3D11_TEXTURE2D_DESC texDesc{};
    texDesc.Width  = width;
    texDesc.Height = height;
    texDesc.MipLevels = 1;
    texDesc.ArraySize = 1;
    texDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    texDesc.SampleDesc.Count = 1;
    texDesc.Usage = D3D11_USAGE_IMMUTABLE;
    texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

    D3D11_SUBRESOURCE_DATA initData{};
    initData.pSysMem = rgbaData;
    initData.SysMemPitch = rowPitchBytes;

    ThrowIfFailed(device->CreateTexture2D(
        &texDesc, &initData, texture.texture_.GetAddressOf()),
        "Texture2D::CreateRGBA8: CreateTexture2D failed.");

    ThrowIfFailed(device->CreateShaderResourceView(
        texture.texture_.Get(), nullptr, texture.srv_.GetAddressOf()),
        "Texture2D::CreateRGBA8: CreateShaderResourceView failed.");

    return texture;
}

} // namespace Salt2D::Render
