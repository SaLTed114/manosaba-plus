// Render/Primitives/FullScreenTexDemo.cpp
#include "FullScreenTexDemo.h"
#include "Render/Shader/ShaderManager.h"
#include "RHI/DX11/DX11Common.h"

#include <vector>
#include <cstdint>
#include <stdexcept>

namespace Salt2D::Render {

static std::vector<uint8_t> MakeCheckerRGBA(uint32_t w, uint32_t h) {
    std::vector<uint8_t> img(w * h * 4);
    for (uint32_t y = 0; y < h; ++y) {
        for (uint32_t x = 0; x < w; ++x) {
            bool c = ((x / 16) ^ (y / 16)) & 1;
            uint8_t v = c ? 255 : 30;

            size_t idx = (y * w + x) * 4;
            img[idx + 0] = v;
            img[idx + 1] = (uint8_t)(x * 255 / (w - 1));
            img[idx + 2] = (uint8_t)(y * 255 / (h - 1));
            img[idx + 3] = 255;
        }
    }
    return img;
}

void FullScreenTexDemo::Initialize(ID3D11Device* device, ShaderManager& shaderMgr) {
    auto vsBlob = shaderMgr.LoadShader("fullscreen_tex_vs.cso");
    auto psBlob = shaderMgr.LoadShader("fullscreen_tex_ps.cso");

    if (!vsBlob || !psBlob) throw std::runtime_error("Failed to load shaders for FullScreenTexDemo.");

    ThrowIfFailed(device->CreateVertexShader(
        vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(),
        nullptr, this->vs.GetAddressOf()),
        "FullScreenTexDemo::Initialize: CreateVertexShader failed.");

    ThrowIfFailed(device->CreatePixelShader(
        psBlob->GetBufferPointer(), psBlob->GetBufferSize(),
        nullptr, this->ps.GetAddressOf()),
        "FullScreenTexDemo::Initialize: CreatePixelShader failed.");

    D3D11_SAMPLER_DESC sampDesc{};
    sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
    sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.MaxLOD = D3D11_FLOAT32_MAX;

    ThrowIfFailed(device->CreateSamplerState(
        &sampDesc, samp_.GetAddressOf()),
        "FullScreenTexDemo::Initialize: CreateSamplerState failed.");

    const uint32_t texW = 640;
    const uint32_t texH = 360;
    auto imgData = MakeCheckerRGBA(texW, texH);
    texture_ = Texture2D::CreateRGBA8(device, texW, texH, imgData.data(), texW * 4);
}

void FullScreenTexDemo::Draw(ID3D11DeviceContext* context, ID3D11RenderTargetView* /*rtv*/) {
    context->IASetInputLayout(nullptr);
    context->IASetVertexBuffers(0, 0, nullptr, nullptr, nullptr);
    context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    context->VSSetShader(vs.Get(), nullptr, 0);
    context->PSSetShader(ps.Get(), nullptr, 0);

    auto srv = texture_.GetSRV();
    ID3D11ShaderResourceView* srvs[] = { srv.Get() };
    context->PSSetShaderResources(0, 1, srvs);

    ID3D11SamplerState* samplers[] = { samp_.Get() };
    context->PSSetSamplers(0, 1, samplers);

    context->Draw(3, 0);
}
    
} // namespace Salt2D::Render
